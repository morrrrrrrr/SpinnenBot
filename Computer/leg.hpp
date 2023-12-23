/*
 *  Autor: MW
 *  Datum: 18/05/23 - made header-only and rewritten on 23/12/2023
 *  Zweck: Include Datei für Spinnenbein
 *  Datei: leg.hpp - previously Leg.cpp/h
 */

#include "constants.hpp"
#include <cmath>

#ifndef LEG_HPP
#define LEG_HPP

#define M_PI 3.14159265358979323846 /* pi */

class Leg_rewrite {
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
    Leg_rewrite(uint8_t index, float angle, Vector3f axis_lengths, Vector3f servo_offsets) : 
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
            m_target_point.at(0)*m_target_point.at(0) + m_target_point.at(2)*m_target_point.at(2)
        ) - m_axis_lengths.at(0);
        float y = m_target_point.at(1); // y part of the target point

        float p0 = std::atan2(m_target_point.at(2), m_target_point.at(0)); // Angle between local x and z -> angle for servo 0
        float b = std::atan2(y, x); // Angle beta

        float l1 = m_axis_lengths.at(1), l2 = m_axis_lengths.at(2);

        float a = std::acos(
            (l1*l1 + l2*l2 - x*x - y*y) / (2*l1*l2)
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
};

#include <tuple> // only the old leg is dependent on this
class Leg {
    uint8_t index;
    float l0, l1, l2;
    float s0Offset, s1Offset, s2Offset;
    float x, y, z; // target X,Y,Z; relative pos: global angle already calculated
    float angle;   // offset angle
public:
    Leg() {
        this->index = 255;
        this->angle = 0;
        this->l0 = 0;
        this->l1 = 0;
        this->l2 = 0;
        this->s0Offset = 0;
        this->s1Offset = 0;
        this->s2Offset = 0;
    }
    Leg(uint8_t index, float angle, float l0, float l1, float l2, float s0Offset, float s1Offset, float s2Offset) {
        this->index = index;
        this->angle = angle;

        this->l0 = l0;
        this->l1 = l1;
        this->l2 = l2;

        this->s0Offset = s0Offset;
        this->s1Offset = s1Offset;
        this->s2Offset = s2Offset;
    }


    void begin() {
        
    }
    std::tuple<float, float, float> update() {
        float x = std::sqrt(this->x*this->x + this->z*this->z) - l0;
        float p0 = std::atan2(this->z, this->x); // Angle between local x and z -> angle for servo 0
        float b = std::atan2(y, x); // Angle beta

        float a = std::acos((l1*l1 + l2*l2 - x*x - y*y) / (2*l1*l2)); // Angle alpha between axises
        float p2 = M_PI - a;

        float psi = std::atan2(l2 * std::sin(p2), l1 + l2 * std::cos(p2));
        float p1 = b + psi;

        // p0 * 180 / M_PI + this->s0Offset;
        // p1 * 180 / M_PI + this->s1Offset;
        // p2 * 180 / M_PI + this->s1Offset;

        return {
            p0 * 180 / M_PI + this->s0Offset,
            p1 * 180 / M_PI + this->s1Offset,
            p2 * 180 / M_PI + this->s1Offset
        };
    }

    // this automatically converts the global-space input vector to a local-space vector
    void setPosition(float x, float y, float z) {
        // rotate from global space to local space
        float a = (angle - 90) * M_PI/180;
        this->x =  x * cos(a) + z * sin(a);
        this->y =  y;
        this->z = -x * sin(a) + z * cos(a);
    }
};

#endif