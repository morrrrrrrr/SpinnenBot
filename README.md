# Hexapod

Code for a 6-legged spider bot

## Table of Contents

- [Coordinates](#coordinates)
- [Movement](#movement)
- [Saving and Loading](#saving-and-loading)
- [Walk Cycle](#walk-cycle)
- [Input](#input)
- [States](#robot-states)
- [Settings](#settings)

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

All Legs move in a circular pattern around the center point

## Saving and Loading

Currently, the leg and the hexapod class have a function for loading and saving from/to a nlohmann::json object

## Walk Cycle

The legs move based on the `delta time`

Legs have a radius around their resting position, in that they can move freely around

When a leg leaves this circle, it transitions from being on ground to being in the air (or the other way around)

When a leg is in the air, it mirrows the movement on ground

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

Vector2f pos = (Vector2f{lin, rot} * speed).normalized(); // all inputs always add up to 100%
```

### Edge Case: All Inputs are 0

When all Inputs are 0, the speed is also set to 0, so nothing moves

Maybe then after 1 second start moving all legs to their origin position

## Robot States

When connected, the robot always is in one of these 3 States:

- __Sleeping__: None of the legs touch the ground. The robot sits on it's chasis
- __Standing__: All of the legs touch the ground: The robot is move-ready
- __Walking__: Walk cycle is executed

### Gaits

When switching from standing to walking, at first a GAIT is initiated

The Gait tells the legs, where they should start

### Stopping Movement

When stopping movement, the user has an X-second delay, before the robot automatically moves back to standing

While the robot transitions no movement can be done

## Settings

These are the settings that modify the behavior of the hexapod robot

They are stored in a json in the hexapod-class

The string in the brackets is the key

### Speed ("speed")

Speed controls how many seconds per walk cycle.

A speed of 1 means, that (on full throttle) the robot takes one walk cycle per second
A speed of 0.1 means 10 seconds per walk cycle

### Linear Distance ("step_dist")

How many millimeters can the leg go away from its resting position (per cycle)

### Ground Percent ("ground_percent")

How much percent of the walk cycle is on the ground

### Robot Height ("rob_height")

How many millimeters does the robot hover over the ground

### Step Height ("step_height")

Peak height of the walk cycle (relative to ground)
