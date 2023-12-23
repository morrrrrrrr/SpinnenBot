/*
 *  Autor: MW
 *  Datum: 18/05/23 - made header-only on 23/12/2023
 *  Zweck: Include Datei für Spinnenbein
 *  Datei: leg.hpp - previously Leg.cpp/h
 */

#include "constants.hpp"
#include "vector.hpp"
#include <tuple>
#include <cmath>

#ifndef LEG_HPP
#define LEG_HPP

#define M_PI 3.14159265358979323846  /* pi */

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