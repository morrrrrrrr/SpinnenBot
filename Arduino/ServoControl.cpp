/*
 *  Autor: Daniel Paar
 *  Datum: 22/05/23
 *  Zweck: Implementation der Servo-Kontrollklasse
 *  Datei: ServoControl.cpp
 */

#include "ServoControl.h"

void ServoControl::switchSlots() {
  uint8_t temp = *slots;
  uint8_t *p = slots, *q = slots + 1;
  for (; q - slots < SERVO_COUNT; ++p, ++q) {
    *p = *q;
  }
  slots[17] = temp;
}

ServoControl::ServoControl() {
  // initialize the angles to 0
  for (float *f = angles; f - angles < SERVO_COUNT; ++f) {
    *f = 0;
  }
  // clear all timers
  for (int i = 0; i < MAX_SERVOS_MOVING; ++i) {
    timer[i].set(0);
  }
  // initialize slots array
  for (int i = 0; i < SERVO_COUNT; i++) {
    slots[i] = i;
  }
}

int ServoControl::begin(uint8_t *pins) {
  for (int i = 0; i < SERVO_COUNT; ++i) {
    while (!Serial.available()) { }
    Serial.read();
    
    servos[i].attach(pins[i]);
    // wait between each servo initialization
  }
}

int ServoControl::update() {
  for (int i = 0; i < MAX_SERVOS_MOVING; i++) {
    if (timer[i].isElapsed()) {
      uint8_t servo = *slots;
      timer[i].set(SERVO_MOVE_TIME);
      if (servo < 3) {
        servos[servo].write(angles[servo]);
      }
      switchSlots();
    }
  }
}

void ServoControl::setAngle(uint8_t servo, float angle) {
  this->angles[servo] = angle;

  // Serial.print("Servo ");
  // Serial.print(servo);
  // Serial.print(": ");
  // Serial.println(angles[servo]);
}

void ServoControl::setAngle(uint8_t leg, uint8_t servo, float angle) {
  this->setAngle(3*leg + servo, angle);

  // Serial.print("Leg: ");
  // Serial.print(leg);
  // Serial.print(", Servo: ");
  // Serial.println(servo);
}