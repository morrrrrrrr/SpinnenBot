/*
*   Morrrr
*    header-only rewrite of Hexapod.cpp/h
*    rewritten on 23/12/23
*/

#include "constants.hpp"
#include "nlohmann/json.hpp"
#include "leg.hpp"

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
    std::array<Leg, 6> m_legs;
    std::array<nlohmann::json, 6> m_leg_data;

    nlohmann::json m_data;

    std::array<float, 18> m_servo_angles;

    vector::Vector<6, Vector2f> m_leg_position;
    vector::Vector<6, bool> m_leg_grounded;

    void moveLeg(int i, Vector2f vec) {
        Vector2f resting_pos = static_cast<Vector2f>(m_leg_data[i].at("rest_position").get<std::array<float, 2>>());
        float radius = m_data.at("step_dist").get<float>();

        if (m_leg_grounded[i]) m_leg_position[i] += vec;
        else                   m_leg_position[i] -= vec;

        Vector2f diff = m_leg_position[i] - resting_pos;
        float diff_mag = diff.magnitude();
        if (diff_mag > radius) {
            m_leg_grounded[i] = !m_leg_grounded[i];

            // normalize diff
            diff.normalize();

            m_leg_position[i] = resting_pos + diff * radius;

            moveLeg(i, diff.inverted() * (diff_mag - radius));
        }
    }

    void applyLinearCycle(float delta) {

    }
    void applyRotationalCycle(float delta) {

    }

public:
    Hexapod() {
        
    }

    // @param delta is in milliseconds
    // @param rot can be negative for turning in the other direction
    void updateWalkCycles(float delta, float lin, float rot, Vector2f linear_direction) {
        float speed = std::max(lin, std::abs(rot));
        Vector2f pos = (Vector2f{lin, rot} * speed).normalized();

        float dt = delta * speed * m_data.at("speed").get<float>();

        applyLinearCycle(dt * pos.at(0));
        applyRotationalCycle(dt * pos.at(1));
    }

    // only call while the robot is resting
    // sets the resting positions of each leg on a circle around the center point of the robot
    // @param radius is the radius of the circle
    void calculateRestingPositions(float radius) {
        for (int i = 0; i < 6; i++) {
            float angle = m_legs[i].getAngle() + m_leg_data[i].at("rest_angle_offset").get<float>();
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
    void load(const nlohmann::json& data) {
        for (int i = 0; i < 6; i++) {
            m_legs[i].load(data["legs"][i]["leg"]);
            m_leg_data[i] = data["legs"][i]["data"];
        }

        m_data = data["data"];
    }
};

#endif