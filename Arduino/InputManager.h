
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

enum States {
  IDLE, RECIEVE_SIGN, RECIEVE_NUM, EVALUATE
};
enum Signs {
  POSITIVE, NEGATIVE
};

class InputManager {
  uint8_t state;
  char  command;
  char *p;
  char inputBuffer[10];
  uint8_t commaPosition;
  bool hadComma;
  bool inputNumSign;
  float inputNum;
public:
  InputManager();

  int begin();
  int update();
};

#endif