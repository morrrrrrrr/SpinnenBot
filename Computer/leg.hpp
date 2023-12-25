/*
 *  Autor: MW
 *  Datum: 18/05/23 - made header-only and rewritten on 23/12/2023
 *  Zweck: Include Datei für Spinnenbein
 *  Datei: leg.hpp - previously Leg.cpp/h
 */

#include "constants.hpp"
#include "nlohmann/json.hpp"
#include <cmath>
#include <string>

#ifndef LEG_HPP
#define LEG_HPP

class Leg {
    // in which angle does the local coordinate system stand to the global
    // used for converting global space points into local space
    float m_angle;

    Vector3f m_axis_lengths;

    // the taget point is in local space.
    Vector3f m_target_point;

public:
    // Simple Constructor
    Leg() { }
    
    Leg(float angle, Vector3f axis_lengths, Vector3f servo_offsets) : 
        m_angle(angle), m_axis_lengths(axis_lengths), m_target_point({0, 0, 0}) {  }
    Leg(const nlohmann::json& data) : m_target_point({0, 0, 0}) {
        load(data);
    }

    // Set the target position of the leg (input point is in global coordinates)
    void setTarget(const Vector3f& target) {
        m_target_point = target;
    }

    // Get a modifiable reference to the relative angle to the robot forward
    float&    getAngle() { return m_angle; }
    // Get a modifiable reference to the vector of the axis lengths
    Vector3f& getAxisLengths() { return m_axis_lengths; }

    // Get a non-modifiable reference to the angle
    const float&    getAngle() const { return m_angle; }
    // Get a non-modifiable reference to the vector of the axis lengths
    const Vector3f& getAxisLengths() const { return m_axis_lengths; }
    // Get a non-modifiable reference to the target vector (that is stored in local space to the leg coordinates)
    const Vector3f& getTarget() const { return m_target_point; }

    // Calculate the axis angles based on inverse kinematic
    Vector3f calculate() const {
        float x = std::sqrt(
            square(m_target_point.at(0)) + square(m_target_point.at(2))
        ) - m_axis_lengths.at(0);
        float y = m_target_point.at(1); // y part of the target point

        float p0 = std::atan2(m_target_point.at(2), m_target_point.at(0)); // Angle between local x and z -> angle for servo 0
        float b = std::atan2(y, x); // Angle beta

        float l1 = m_axis_lengths.at(1), l2 = m_axis_lengths.at(2);

        float a = std::acos(
            (square(l1) + square(l2) - square(x) - square(y)) / (2*l1*l2)
        ); // Angle alpha between axises
        float p2 = M_PI - a;

        float psi = std::atan2(l2 * std::sin(p2), l1 + l2 * std::cos(p2));
        float p1 = b + psi;

        return Vector3f{
            p0 * 180 / static_cast<float>(M_PI),
            p1 * 180 / static_cast<float>(M_PI),
            p2 * 180 / static_cast<float>(M_PI)
        };
    }

    // Set a target, then calculate the angles
    Vector3f calculate(const Vector3f& target) {
        setTarget(target);
        return calculate();
    }

    // save leg settings to a json object
    nlohmann::json save() const {
        nlohmann::json data;

        data["angle"] = m_angle;
        data["axis_lengths"] = std::array<float, 3>{
            m_axis_lengths.at(0), m_axis_lengths.at(1), m_axis_lengths.at(2)
        };

        return data;
    }
    // load leg settings from a json object
    void load(const nlohmann::json& data) {
        m_angle = data.at("angle").get<float>();
        m_axis_lengths = static_cast<Vector3f>(data.at("axis_lengths").get<std::array<float, 3>>());
    }
};

#endif