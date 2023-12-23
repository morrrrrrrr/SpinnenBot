# Hexapod

Code for a 6-legged spider bot

## Table of Contents

- [Coordinates](#coordinates)
- [Movement](#movement)
- [Saving and Loading](#saving-and-loading)

## Coordinates

Y is the height (`up`)
X is width (`right`)
Z is depth (`forward`)

### Global Coordinates

Global Coordinates are always in reference to the robot's center point and forward vector

### Local Coordinates

Local Coordinates are used for the angle calculations of the legs.

The Leg-Class has a function, that takes in a global position and converts it into its own local system

## Movement

There are 2 different kinds of movement:

- [Linear](#linear-movement)
- [Rotational](#rotational-movement)

### Linear Movement

All Legs move in the same direction when grounded

### Rotational Movement

All Legs move

## Saving and Loading

Currently, the leg and the hexapod class have a function for loading and saving from/to a nlohmann::json object
