/*
 *  Autor: MW
 *  Datum: 02/03/23
 *  Datei: SimpleTimer.h
 *  Zweck: Include Datei für Timerklasse at ali.
 */

#ifndef SIMPLE_TIMER_H
#define SIMPLE_TIMER_H

class SimpleTimer {
  unsigned long startTime;
  unsigned long duration;

  bool running;
public:
  SimpleTimer();

  void set(unsigned long duration);
  bool isElapsed(bool stopTimer = true);
  bool isRunning();
  void stop();

  unsigned long current();
  unsigned long remaining();
};

#endif