/*
*   Morrrr
*    header-only rewrite of Hexapod.cpp/h
*    rewritten on 23/12/23
*/

#include "constants.hpp"
#include "leg.hpp"

#include <tuple>
#include <array>

#ifndef HEXAPOD_HPP
#define HEXAPOD_HPP

// The Hexapod class houses the control of the target points of the legs
class Hexapod {
    // the legs are stored with a respective vector, that is the offset of the leg origin from the robot center
    std::array<std::pair<Leg, Vector2f>, 6> m_legs;
    
public:

};

#endif