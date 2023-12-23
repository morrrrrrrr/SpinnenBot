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

#ifndef HEXAPOD_HPP
#define HEXAPOD_HPP

// The Hexapod class houses the control of the target points of the legs
class Hexapod {
    std::array<Leg, 6> m_legs;
    std::array<nlohmann::json, 6> m_leg_data;

    std::array<float, 18> m_servo_angles;

public:
    Hexapod() {
        Vector3f axis_lengths{ 60.05, 73, 62 }, servo_offsets{ 0, 0, 0 };
        m_legs[0] = Leg( 31.315, axis_lengths, servo_offsets);
        m_legs[1] = Leg( 90.000, axis_lengths, servo_offsets);
        m_legs[2] = Leg(148.685, axis_lengths, servo_offsets);
        m_legs[3] = Leg(211.315, axis_lengths, servo_offsets);
        m_legs[4] = Leg(270.000, axis_lengths, servo_offsets);
        m_legs[5] = Leg(328.685, axis_lengths, servo_offsets);

        m_leg_data[0] = {
            {"rest_angle_offset", 0},
            {"origin_offset", std::array<float, 2>{
                0, 0
            }}
        };
        m_leg_data[1] = {
            {"rest_angle_offset", 0},
            {"origin_offset", std::array<float, 2>{
                0, 0
            }}
        };
        m_leg_data[2] = {
            {"rest_angle_offset", 0},
            {"origin_offset", std::array<float, 2>{
                0, 0
            }}
        };
        m_leg_data[3] = {
            {"rest_angle_offset", 0},
            {"origin_offset", std::array<float, 2>{
                0, 0
            }}
        };
        m_leg_data[4] = {
            {"rest_angle_offset", 0},
            {"origin_offset", std::array<float, 2>{
                0, 0
            }}
        };
        m_leg_data[5] = {
            {"rest_angle_offset", 0},
            {"origin_offset", std::array<float, 2>{
                0, 0
            }}
        };
    }

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
        
        return data;
    }
    void load(const nlohmann::json& data) {
        for (int i = 0; i < 6; i++) {
            m_legs[i].load(data["legs"][i]["leg"]);
            m_leg_data[i] = data["legs"][i]["data"];
        }
    }
};

#endif