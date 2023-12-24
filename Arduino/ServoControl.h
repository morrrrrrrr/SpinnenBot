/*
 *  Autor: Daniel Paar
 *  Datum: 22/05/23
 *  Zweck: Include Datei der Servo-Kontrollklasse
 *  Datei: ServoControl.h
 */

#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <Servo.h>
#include "SimpleTimer.h"

#define SERVO_COUNT 18
#define MAX_SERVOS_MOVING 3
#define SERVO_MOVE_TIME 0

class ServoControl {
private:
  Servo servos[SERVO_COUNT];
  float angles[SERVO_COUNT];
  SimpleTimer timer[MAX_SERVOS_MOVING];
  uint8_t slots[SERVO_COUNT];

  void switchSlots();
public:
  ServoControl();
  int begin(uint8_t pins[]);
  int update();
  void setAngle(uint8_t servo, float angle);
  void setAngle(uint8_t leg, uint8_t servo, float angle);
};

#endif