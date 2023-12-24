
#include "ServoControl.h"
#include "InputManager.h"

extern ServoControl servoControl;

float pow10(int n) {
  if (n > 0) {
    float out = 1;
    for (int i = 0; i < n; i++) { out *= 10; }
    return out;
  }
  if (n < 0) {
    float out = 1;
    for (int i = 0; i > n; i--) { out /= 10; };
    return out;
  }
  return 1;
}

InputManager::InputManager() {
  p = inputBuffer;
}

int InputManager::begin() {

}

int InputManager::update() {
  switch (state) {
    case IDLE: {
      if (Serial.available()) {
        char c = Serial.read();
        command = c;
        state = RECIEVE_SIGN;
      }
      break;
    }
    case RECIEVE_SIGN: {
      if (Serial.available()) {
        char c = Serial.read();
        p = inputBuffer;
        switch (c) {
          case '-': {
            inputNumSign = NEGATIVE;
            inputNum = 0;
            commaPosition = 0;
            hadComma = 0;
            state = RECIEVE_NUM;
            break;
          }
          default: {
            if (c >= '0' && c <= '9') {
              *p = c;
              p++;
              inputNum = 0;
              inputNumSign = POSITIVE;
              commaPosition = 1;
              hadComma = 0;
              state = RECIEVE_NUM;
            }
            else {
              Serial.println("No Number given. State is now IDLE again");
              state = IDLE;
            }
            break;
          }
        }
      }
      break;
    }
    case RECIEVE_NUM: {
      if (Serial.available()) {
        char c = Serial.read();
        if (c >= '0' && c <= '9') {
          *p = c;
          p++;
          if (!hadComma)
            commaPosition++;
        }
        else if ((c == '.' || c == ',') && p != inputBuffer) {
          commaPosition = p - inputBuffer;
          hadComma = 1;
        }
        else if (c == '.' || c == ',') {
          Serial.println("Had a comma on the first position. Returning state to Idle");
          state = IDLE;
        }
        else {
          *p = 0;
          state = EVALUATE;
        }
      }
      break;
    }
    case EVALUATE: {
      for (char *q = inputBuffer; *q; ++q) {
        inputNum += (*q - '0') * pow10(p - q - 1 - (p - inputBuffer - commaPosition));
      }
      inputNum *= ((inputNumSign == POSITIVE) ? 1 : -1);
      p = inputBuffer;

      servoControl.setAngle(command - 'a', inputNum);
      
      state = IDLE;
      break;
    }
  }
}
