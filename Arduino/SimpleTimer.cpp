/*
 *  Autor: MW
 *  Datum: 02/03/23
 *  Datei: SimpleTimer.cpp
 *  Zweck: Timerklasse at ali.
 */

#include "SimpleTimer.h"
#include "Arduino.h"

SimpleTimer::SimpleTimer() {
  this->startTime = 0;
  this->duration = 0;
  this->running = false;
}

void SimpleTimer::set(unsigned long duration) {
  this->startTime = millis();
  this->duration = duration;
  this->running = true;
}

bool SimpleTimer::isElapsed(bool stopTimer = true) {
  bool val = millis() >= this->startTime + this->duration && this->running;
  this->running = !(stopTimer && val);
  return val;
}

bool SimpleTimer::isRunning() {
  return this->running;
}

void SimpleTimer::stop() {
  this->running = false;
}

unsigned long SimpleTimer::current() {
  return millis() - startTime;
}

unsigned long SimpleTimer::remaining() {
  if (this->isElapsed(false))
    return startTime + duration - millis();
  else return 0;
}
