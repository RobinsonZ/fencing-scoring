#include "include/epee.h"

#include <Arduino.h>

#include "include/constants.h"
#include "include/scorelights.h"

#define WAIT 1
#define LEFT_PRETOUCH 2
#define LEFT_WAIT 3
#define LEFT_DOUBLE_PRETOUCH 4
#define RIGHT_PRETOUCH 5
#define RIGHT_WAIT 6
#define RIGHT_DOUBLE_PRETOUCH 7

// all the targets must be contiguous
#define LEFT_TOUCH 8
#define RIGHT_TOUCH 9
#define DOUBLE_TOUCH 10

#ifndef fence_timers
#define fence_timers
unsigned long epeeTimerMain, epeeTimerSecondary;
unsigned int epeeState = WAIT;
unsigned int epeeLastState = 0;
#endif

void epee_setup() {
  // outputs
  pinMode(LEFT_B, OUTPUT);
  pinMode(RIGHT_B, OUTPUT);

  // inputs
  pinMode(LEFT_A, INPUT_PULLUP);
  pinMode(LEFT_C, INPUT_PULLUP);
  pinMode(RIGHT_A, INPUT_PULLUP);
  pinMode(RIGHT_C, INPUT_PULLUP);

  digitalWrite(LEFT_B, LOW);
  digitalWrite(RIGHT_B, LOW);

  initBlink();
}

void setGndLights(bool left_gnd, bool right_gnd) {
  setLeftGnd(left_gnd);
  setRightGnd(right_gnd);
}

void setLeftGnd(bool gnd) {
  if (gnd) {
    setLeft(LIGHTS_GND);
  } else {
    setLeft(LIGHTS_OFF);
  }
}

void setRightGnd(bool gnd) {
  if (gnd) {
    setRight(LIGHTS_GND);
  } else {
    setRight(LIGHTS_OFF);
  }
}

void epee_state() {
  // pins will be pulled LOW if there is a short between them and LEFT_B
  bool left_contact = !digitalRead(LEFT_A);
  bool right_contact = !digitalRead(RIGHT_A);
  // whether weapon is grounded each side
  bool right_gnd = !digitalRead(LEFT_C);
  bool left_gnd = !digitalRead(RIGHT_C);

  if (epeeState != epeeLastState) {
    Serial.println(epeeState);
    epeeLastState = epeeState;
  }
  switch (epeeState) {
    case WAIT:
      setGndLights(left_gnd, right_gnd);

      if (left_contact && !left_gnd) {
        epeeTimerMain = micros();
        epeeState = LEFT_PRETOUCH;
      } else if (right_contact && !right_gnd) {
        epeeTimerMain = micros();
        epeeState = RIGHT_PRETOUCH;
      }  // otherwise stay in WAIT
      break;
    case LEFT_PRETOUCH:
      setGndLights(left_gnd, right_gnd);

      if (left_contact && !left_gnd) {
        if (micros() - epeeTimerMain >= EPEE_MIN_TOUCH * 1000) {
          epeeTimerMain = micros();
          epeeState = LEFT_WAIT;
        }  // otherwise stay here
      } else {
        // button got released
        epeeState = WAIT;
      }

      break;
    case LEFT_WAIT:
      setLeft(LIGHTS_TGT);
      setRightGnd(left_gnd);

      if (right_contact && !right_gnd) {
        epeeTimerSecondary = micros();
        epeeState = LEFT_DOUBLE_PRETOUCH;
      } else if (micros() - epeeTimerMain >= EPEE_DOUBLE_PERIOD * 1000L) {
        epeeTimerMain = micros();
        epeeState = LEFT_TOUCH;
      }  // otherwise stay here
      break;
    case LEFT_DOUBLE_PRETOUCH:
      setLeft(LIGHTS_TGT);
      setRightGnd(right_gnd);

      if (right_contact && !right_gnd) {
        if (micros() - epeeTimerSecondary >= EPEE_MIN_TOUCH * 1000L) {
          epeeTimerMain = micros();
          epeeState = DOUBLE_TOUCH;
        }  // otherwise do nothing
      } else {
        epeeState = LEFT_WAIT;
      }
      break;
    case RIGHT_PRETOUCH:
      setGndLights(left_gnd, right_gnd);

      if (right_contact && !right_gnd) {
        if (micros() - epeeTimerMain >= EPEE_MIN_TOUCH * 1000) {
          epeeTimerMain = micros();
          epeeState = RIGHT_WAIT;
        }  // otherwise stay here
      } else {
        // button got released
        epeeState = WAIT;
      }

      break;
    case RIGHT_WAIT:
      setRight(LIGHTS_TGT);
      setLeftGnd(left_gnd);

      if (left_contact && !left_gnd) {
        epeeTimerSecondary = micros();
        epeeState = RIGHT_DOUBLE_PRETOUCH;
      } else if (micros() - epeeTimerMain >= EPEE_DOUBLE_PERIOD * 1000L) {
        epeeTimerMain = micros();
        epeeState = RIGHT_TOUCH;
      }  // otherwise stay here
      break;
    case RIGHT_DOUBLE_PRETOUCH:
      setRight(LIGHTS_TGT);
      setLeftGnd(left_gnd);

      if (left_contact && !left_gnd) {
        if (micros() - epeeTimerSecondary >= EPEE_MIN_TOUCH * 1000L) {
          epeeTimerMain = micros();
          epeeState = DOUBLE_TOUCH;
        }  // otherwise do nothing
      } else {
        epeeState = RIGHT_WAIT;
      }
      break;
    case LEFT_TOUCH:
      setLeft(LIGHTS_TGT);
      setRightGnd(right_gnd);

      if (micros() - epeeTimerMain >= REARM_TIME * 1000L) {
        epeeState = WAIT;
      }
      break;
    case RIGHT_TOUCH:
      setRight(LIGHTS_TGT);
      setLeftGnd(left_gnd);

      if (micros() - epeeTimerMain >= REARM_TIME * 1000L) {
        epeeState = WAIT;
      }
      break;
    case DOUBLE_TOUCH:
      setLeft(LIGHTS_TGT);
      setRight(LIGHTS_TGT);

      if (micros() - epeeTimerMain >= REARM_TIME * 1000L) {
        epeeState = WAIT;
      }
      break;
  }
}
