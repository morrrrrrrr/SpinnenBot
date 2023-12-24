
#include <Arduino.h>
#include "ServoControl.h"
#include "InputManager.h"

ServoControl servoControl;
InputManager inputManager;

// TODO: fill with all 18 pins
uint8_t pins[]{
  1
};

void setup() {
  Serial.begin(9600);
  servoControl.begin(pins);
  inputManager.begin();
}

void loop() {
  servoControl.update();
  inputManager.update();
}
