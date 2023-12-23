# Hexapod

Code for a 6-legged spider bot

## Table of Contents

- [Coordinates](#coordinates)
- [Movement](#movement)
- [Saving and Loading](#saving-and-loading)

## Coordinates

- Y is the height (`up`)
- X is width (`right`)
- Z is depth (`forward`)

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

## Walk Cycle

A walk cycle is the movement of a leg based on the time

Any walk cylce takes 1000 ms (1s)

Walk cycles can be added, to overlay them

There are 3 parameters for each walk cycle:

- __t__: time in milliseconds
- __pos__: how much ground is cleared by the walk cycle in percent (when there are multiple walk cycles, the pos% of them all must add up to 100%)
- __speed__: how fast is the walk cycle updated in percent (is the maximum percent from all of the inputs)

## Input

Inputs are for each walk cycle different

for example `linear` and `rotational` walk cycles:

an input vector for that could look like that:

- lin=50%
- rot=100%

### Calculate Pos and Speed

```cpp
float lin=0.5;
float rot=1.0;

float speed = max(lin, rot); // speed can never go over 100%

Vector2f pos = (Vector2f{lin, rot} * speed).normalized() // all inputs always add up to 100%
```

### Edge Case: All Inputs are 0

When all Inputs are 0, the speed is also set to 0, so nothing moves

Maybe then after 1 second start moving all legs to their origin position
