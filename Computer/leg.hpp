/*
 *  Autor: MW
 *  Datum: 18/05/23 - made header-only and rewritten on 23/12/2023
 *  Zweck: Include Datei für Spinnenbein
 *  Datei: leg.hpp - previously Leg.cpp/h
 */

#include "constants.hpp"
#include <cmath>
#include <string>

#ifndef LEG_HPP
#define LEG_HPP

#define M_PI 3.14159265358979323846 /* pi */

class Leg {
    uint8_t m_index;
    
    // in which angle does the local coordinate system stand to the global
    // used for converting global space points into local space
    float m_angle;

    Vector3f m_axis_lengths;
    Vector3f m_servo_offsets;

    // the taget point is in local space.
    Vector3f m_target_point;

public:
    // Simple Constructor
    Leg(uint8_t index, float angle, Vector3f axis_lengths, Vector3f servo_offsets) : 
        m_index(index), m_angle(angle), m_axis_lengths(axis_lengths), m_servo_offsets(servo_offsets), m_target_point({0, 0, 0}) {  }

    // Set the target position of the leg (input point is in global coordinates)
    void setTarget(const Vector3f& target) {
        m_target_point = target;
    }

    // Get a modifiable reference to the relative angle to the robot forward
    float&    getAngle() { return m_angle; }
    // Get a modifiable reference to the vector of the axis lengths
    Vector3f& getAxisLengths() { return m_axis_lengths; }
    // Get a modifiable reference to the vector that stores the servo offsets
    Vector3f& getServoOffsets() { return m_servo_offsets; }
    // Get a non-modifiable reference to the target vector (that is stored in local space to the leg coordinates)
    Vector3f getTarget() const { return m_target_point; }

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
            p0 * 180 / static_cast<float>(M_PI) + m_servo_offsets.at(0),
            p1 * 180 / static_cast<float>(M_PI) + m_servo_offsets.at(1),
            p2 * 180 / static_cast<float>(M_PI) + m_servo_offsets.at(2)
        };
    }

    // Set a target, then calculate the angles
    Vector3f calculate(const Vector3f& target) {
        setTarget(target);
        return calculate();
    }

    
    std::string save() const {
        
    }
    void load(const std::string& str) {

    }
};

#endif