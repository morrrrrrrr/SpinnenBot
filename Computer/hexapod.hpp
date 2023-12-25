/*
*   Morrrr
*    header-only rewrite of Hexapod.cpp/h
*    rewritten on 23/12/23
*/

#include "constants.hpp"
#include "nlohmann/json.hpp"
#include "leg.hpp"

#include "render/robot_renderer.hpp"

#include <tuple>
#include <array>
#include <cmath>
#include <map>
#include <functional>

#ifndef HEXAPOD_HPP
#define HEXAPOD_HPP

static float map(float a, float b, float c, float d, float val) {
    return (val - a) / (b - a) * (d - c) + c;
}

// The Hexapod class houses the control of the target points of the legs
class Hexapod {
    enum State {
        Sleep, Stand, Walk, // States
        SLEEP_TO_STAND, STAND_TO_SLEEP, WALK_TO_STAND, STAND_TO_WALK // Transitions
    };

    State m_state = State::Sleep;
    float m_transition_timer = 0;

    std::array<Leg, 6> m_legs;
    std::array<nlohmann::json, 6> m_leg_data;

    nlohmann::json m_data;

    // X-Z plane leg position
    vector::Vector<6, Vector2f> m_leg_position;
    // flag, if the leg is on ground
    vector::Vector<6, bool> m_leg_grounded;

    // The height function takes in the distance to the edge of the move circle and returns the height, if the leg is not on ground
    // @param i the index of the leg (used to calculate the distance and check the ground flag)
    // @returns the height of the leg
    float heightFunction(int i) {
        const float rob_height = m_data.at("rob_height").get<float>();
        const float step_height = m_data.at("step_height").get<float>();

        // if the leg is on the ground, it should be 'rob_height' below the robot center
        if (m_leg_grounded[i]) return -rob_height;

        // get resting pos and radius from data
        const Vector2f resting_pos = static_cast<Vector2f>(m_leg_data[i].at("rest_position").get<std::array<float, 2>>());
        const float radius = m_data.at("step_dist").get<float>();

        // distance between pos and edge of the circle
        float dist = std::abs(
            // difference between resting pos and current
            (m_leg_position[i] - resting_pos).magnitude() -
            radius // minus radius
        );

        // maximum distance is radius
        float t = dist / radius;
        // 0 if the pos is directly on the edge                      -> height should be -rob_height
        // 1 if the pos is directly in the center of the walk circle -> height should be -rob_height + step_height

        // this function (1.14 * t*t*t - 3.28 * t*t + 3.14 * t) was constructed in geogebra
        return -rob_height + (1.14 * t*t*t - 3.28 * t*t + 3.14 * t) * step_height;
    }

    // moves a leg of the robot by a distance. 
    // the function toggles the grounded flags and recursivly adds not-used distance when it hits an edge
    // @param i is the leg-index
    // @param vec is the distance moved
    void moveLeg(int i, Vector2f vec) {
        const Vector2f resting_pos = static_cast<Vector2f>(m_leg_data[i].at("rest_position").get<std::array<float, 2>>());
        const float radius = m_data.at("step_dist").get<float>();

        if (m_leg_grounded[i]) m_leg_position[i] += vec;
        else                   m_leg_position[i] -= vec;

        Vector2f diff = m_leg_position[i] - resting_pos;
        float diff_mag = diff.magnitude();
        if (diff_mag > radius) {
            m_leg_grounded[i] = !m_leg_grounded[i];

            // normalize diff
            diff.normalize();

            m_leg_position[i] = resting_pos + diff * radius;

            // moveLeg(i, diff.inverted() * (diff_mag - radius));
        }
    }

    // Applies the linear walk cycle
    // @param delta is the delta time in seconds
    // @param dir is the `normalized` direction vector
    void applyLinearCycle(float delta, const Vector2f& dir) {
        for (int i = 0; i < 6; i++) moveLeg(i, dir * delta);
    }
    // Applies the rotational walk cycle
    // @param delta is the delta time in seconds
    void applyRotationalCycle(float delta) {

    }

    // Initiates a transition
    // @param transition is the transition (needs to be a transition)
    void initiateTransition(State transition) {
        // check, if the transition is possible
        if (!canTransition(transition)) return;

        // reset the timer
        m_transition_timer = 0;

        // set the state
        m_state = transition;
    }

public:
    Hexapod() {
        m_state = State::Walk;
    }

    // Function, that updates the leg positions based on delta time and inputs
    // @param delta is in milliseconds
    // @param lin is the percent of linear walking input
    // @param linear_direction is the direction of linear input; doesn't need to be normalized
    // @param rot is the percent of rotational walking input; can be negative for turning in the other direction
    void update(float delta, float lin, const Vector2f& linear_direction, float rot) {
        float speed = std::max(lin, std::abs(rot));
        Vector2f pos = (Vector2f{lin, rot} * speed).normalized();

        // modify the delta time by the speed stats
        float dt = delta * m_data.at("speed").get<float>();

        m_transition_timer += dt;

        switch (m_state) {
        case State::Sleep:
            // do nothing
            break;
        case State::Stand:
            // do nothing
            break;
        case State::Walk:
            applyLinearCycle(dt * pos.at(0) * speed, linear_direction.normalized());

            // if rot was negative, pos.at(1) is also negative now, so the delta time is negative, so the rotational movement gets reversed
            applyRotationalCycle(dt * pos.at(1) * speed);
            break;
        
        // TRANSITIONS:
        case State::SLEEP_TO_STAND:
            break;
        case State::STAND_TO_SLEEP:
            break;
        case State::WALK_TO_STAND:
            break;
        case State::STAND_TO_WALK:
            break;
        }
    }

    // checks, if the transition could be made
    bool canTransition(State transition) {
        return
            (m_state == State::Sleep && transition == State::SLEEP_TO_STAND) ||
            (m_state == State::Stand && transition == State::STAND_TO_SLEEP) ||
            (m_state == State::Stand && transition == State::STAND_TO_WALK) ||
            (m_state == State::Walk && transition == State::WALK_TO_STAND);
    }

    // Calculates the Servo angles by calling the leg.calculate for every leg
    std::array<float, 18> calculateServoAngles() {
        std::array<float, 18> angles;
        std::array<Vector3f, 6> leg_positions;

        // loop over each leg
        for (int i = 0; i < 6; i++) {
            Vector2f offset = static_cast<Vector2f>(m_leg_data[i].at("origin_offset").get<std::array<float, 2>>());
            Vector3f leg_position{
                m_leg_position[i].at(0), // X
                heightFunction(i),       // Y
                m_leg_position[i].at(1)  // Z
            };

            leg_positions[i] = leg_position;

            // call the calculate version, that also takes in the target position of the leg
            Vector3f leg_angles = m_legs[i].calculate(leg_position - Vector3f{offset.at(0), 0, offset.at(1)});

            // fill in the values in the angles array
            for (int j = 0; j < 3; j++) {
                angles[i * 3 + j] = leg_angles.at(j);
            }
        }

        // send un-offsetted angles to renderer
        RobotRenderer::sendAngles(m_leg_data, m_legs, angles, leg_positions);

        // get offsets from data
        std::array<float, 18> offsets = m_data["servo_offsets"].get<std::array<float, 18>>();

        // apply offsets
        for (int i = 0; i < 18; i++) angles[i] += offsets[i];

        return angles;
    }

    // only call while the robot is resting
    // sets the resting positions of each leg on a circle around the center point of the robot
    // @param radius is the radius of the circle
    void calculateRestingPositions(float radius) {
        for (int i = 0; i < 6; i++) {
            float angle = m_legs[i].getAngle() + m_leg_data[i].at("rest_angle_offset").get<float>();
            
            angle *= (M_PI / 180);
            
            Vector2f direction{ std::cos(angle), std::sin(angle) };
            Vector2f offset = static_cast<Vector2f>(m_leg_data[i].at("origin_offset").get<std::array<float, 2>>());

            // this position is not on the circle yet
            Vector2f test_position = offset + direction * radius;

            // now it is on the circle
            Vector2f rest_position = test_position.normalized() * radius;

            m_leg_data[i]["rest_position"] = std::array<float, 2>{
                rest_position.at(0), rest_position.at(1)
            };
        }
    }

    // Save the configurations to a json object
    nlohmann::json save() {
        nlohmann::json data;
        
        data["legs"] = nlohmann::json::object().array();

        for (int i = 0; i < 6; i++) {
            nlohmann::json leg_save;

            // save leg
            leg_save["leg"] = m_legs[i].save();

            // save extra leg data
            leg_save["data"] = m_leg_data[i];
            
            data["legs"].push_back(leg_save);
        }

        data["data"] = m_data;
        
        return data;
    }
    // Load configurations from a json object
    void load(const nlohmann::json& data) {
        for (int i = 0; i < 6; i++) {
            m_legs[i].load(data["legs"][i]["leg"]);
            m_leg_data[i] = data["legs"][i]["data"];
        }

        m_data = data["data"];

        for (int i = 0; i < 6; i++)
            m_legs[i].getAngle() = 90 - m_legs[i].getAngle();

        calculateRestingPositions(200);

        for (int i = 0; i < 6; i++) {
            // m_leg_grounded[i] = true;
            m_leg_grounded[i] = (i % 2);
            m_leg_position[i] = static_cast<Vector2f>(m_leg_data[i].at("rest_position").get<std::array<float, 2>>());
        }
    }
};

#endif