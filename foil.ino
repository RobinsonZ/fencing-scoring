#include "include/constants.h"
#include "include/foil.h"
#include "include/scorelights.h"

#define WAIT 1
#define LEFT_PRETOUCH 2
#define LEFT_TGT_WAIT 3
#define LEFT_OFFTGT_WAIT 4
#define LEFT_TGT_DOUBLE_PRETOUCH 5
#define LEFT_OFFTGT_DOUBLE_PRETOUCH 6
#define RIGHT_PRETOUCH 7
#define RIGHT_TGT_WAIT 8
#define RIGHT_OFFTGT_WAIT 9
#define RIGHT_TGT_DOUBLE_PRETOUCH 10
#define RIGHT_OFFTGT_DOUBLE_PRETOUCH 11
// all the targets must be contiguous
#define LEFT_TGT 12
#define LEFT_OFFTGT 13
#define RIGHT_TGT 14
#define RIGHT_OFFTGT 15
#define LEFT_TGT_RIGHT_TGT 16
#define LEFT_TGT_RIGHT_OFFTGT 17
#define LEFT_OFFTGT_RIGHT_OFFTGT 18
#define LEFT_OFFTGT_RIGHT_TGT 19

unsigned long timerMain, timerSecondary, timerConsole;
unsigned int state = WAIT;
unsigned int lastState = 0;

void foil_setup() {
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
}

void foil_state() {
  // pins will be pulled LOW if there is a short between them and LEFT_B
  bool left_c = !digitalRead(LEFT_C);
  bool right_c = !digitalRead(RIGHT_C);

  // if (millis() - timerConsole >= 100) {
  //   Serial.print(state);
  //   Serial.print("\t");
  //   Serial.println(micros() - timerMain);
  // }

  if (state != lastState) {
    Serial.println(state);
    lastState = state;
  }
  switch (state) {
    case WAIT:
      // digitalWrite(LEFT_TARGET, LOW);
      // digitalWrite(LEFT_OFF_TARGET, LOW);
      // digitalWrite(RIGHT_TARGET, LOW);
      // digitalWrite(RIGHT_OFF_TARGET, LOW);

      setLeft(LIGHTS_OFF);
      setRight(LIGHTS_OFF);

      if (left_c) {
        timerMain = micros();
        state = LEFT_PRETOUCH;
      } else if (right_c) {
        timerMain = micros();
        state = RIGHT_PRETOUCH;
      }  // otherwise stay in WAIT
      break;
    case LEFT_PRETOUCH:
      if (left_c) {
        if (micros() - timerMain >= FOIL_MIN_TOUCH * 1000) {
          timerMain = micros();
          if (!digitalRead(RIGHT_A)) {
            // on-target
            state = LEFT_TGT_WAIT;
          } else {
            state = LEFT_OFFTGT_WAIT;
          }
        }  // otherwise stay here
      } else {
        // button got released
        state = WAIT;
      }

      break;
    case LEFT_TGT_WAIT:
      setLeft(LIGHTS_TGT);

      if (right_c) {
        timerSecondary = micros();
        state = LEFT_TGT_DOUBLE_PRETOUCH;
      } else if (micros() - timerMain >= FOIL_DOUBLE_PERIOD * 1000L) {
        timerMain = micros();
        state = LEFT_TGT;
      }  // otherwise stay here
      break;
    case LEFT_OFFTGT_WAIT:
      setLeft(LIGHTS_OFFTGT);

      if (right_c) {
        timerSecondary = micros();
        state = LEFT_OFFTGT_DOUBLE_PRETOUCH;
      } else if (micros() - timerMain >= FOIL_DOUBLE_PERIOD * 1000L) {
        timerMain = micros();
        state = LEFT_OFFTGT;
      }  // otherwise stay here
      break;
    case LEFT_TGT_DOUBLE_PRETOUCH:
      setLeft(LIGHTS_TGT);

      if (right_c) {
        if (micros() - timerSecondary >= FOIL_MIN_TOUCH * 1000L) {
          timerMain = micros();
          if (!digitalRead(LEFT_A)) {
            // on-target
            state = LEFT_TGT_RIGHT_TGT;
          } else {
            state = LEFT_TGT_RIGHT_OFFTGT;
          }
        }  // otherwise do nothing
      } else {
        state = LEFT_TGT_WAIT;
      }
      break;
    case LEFT_OFFTGT_DOUBLE_PRETOUCH:
      setLeft(LIGHTS_OFFTGT);

      if (right_c) {
        if (micros() - timerSecondary >= FOIL_MIN_TOUCH * 1000L) {
          timerMain = micros();
          if (!digitalRead(LEFT_A)) {
            // on-target
            state = LEFT_OFFTGT_RIGHT_TGT;
          } else {
            state = LEFT_OFFTGT_RIGHT_OFFTGT;
          }
        }  // otherwise do nothing
      } else {
        state = LEFT_OFFTGT_WAIT;
      }
      break;
    case RIGHT_PRETOUCH:
      if (right_c) {
        if (micros() - timerMain >= FOIL_MIN_TOUCH * 1000) {
          timerMain = micros();
          if (!digitalRead(LEFT_A)) {
            // on-target
            state = RIGHT_TGT_WAIT;
          } else {
            state = RIGHT_OFFTGT_WAIT;
          }
        }  // otherwise stay here
      } else {
        // button got released
        state = WAIT;
      }

      break;
    case RIGHT_TGT_WAIT:
      setRight(LIGHTS_TGT);

      if (left_c) {
        timerSecondary = micros();
        state = RIGHT_TGT_DOUBLE_PRETOUCH;
      } else if (micros() - timerMain >= FOIL_DOUBLE_PERIOD * 1000L) {
        timerMain = micros();
        state = RIGHT_TGT;
      }  // otherwise stay here
      break;
    case RIGHT_OFFTGT_WAIT:
      setRight(LIGHTS_OFFTGT);

      if (left_c) {
        timerSecondary = micros();
        state = RIGHT_OFFTGT_DOUBLE_PRETOUCH;
      } else if (micros() - timerMain >= FOIL_DOUBLE_PERIOD * 1000L) {
        timerMain = micros();
        state = RIGHT_OFFTGT;
      }  // otherwise stay here
      break;
    case RIGHT_TGT_DOUBLE_PRETOUCH:
      setRight(LIGHTS_TGT);

      if (left_c) {
        if (micros() - timerSecondary >= FOIL_MIN_TOUCH * 1000L) {
          timerMain = micros();
          if (!digitalRead(RIGHT_A)) {
            // on-target
            state = LEFT_TGT_RIGHT_TGT;
          } else {
            state = LEFT_OFFTGT_RIGHT_TGT;
          }
        }  // otherwise do nothing
      } else {
        state = RIGHT_TGT_WAIT;
      }
      break;
    case RIGHT_OFFTGT_DOUBLE_PRETOUCH:
      setRight(LIGHTS_OFFTGT);

      if (left_c) {
        if (micros() - timerSecondary >= FOIL_MIN_TOUCH * 1000L) {
          timerMain = micros();
          if (!digitalRead(RIGHT_A)) {
            // on-target
            state = LEFT_TGT_RIGHT_OFFTGT;
          } else {
            state = LEFT_OFFTGT_RIGHT_OFFTGT;
          }
        }  // otherwise do nothing
      } else {
        state = RIGHT_OFFTGT_WAIT;
      }
      break;
    case LEFT_TGT ... LEFT_OFFTGT_RIGHT_TGT:

      if (state == LEFT_TGT || state == LEFT_TGT_RIGHT_TGT ||
          state == LEFT_TGT_RIGHT_OFFTGT) {
        setLeft(LIGHTS_TGT);
      } else if (state == LEFT_OFFTGT || state == LEFT_OFFTGT_RIGHT_TGT ||
          state == LEFT_OFFTGT_RIGHT_OFFTGT) {
        setLeft(LIGHTS_OFFTGT);
      } else {
        setLeft(LIGHTS_OFF);
      }

      if (state == RIGHT_TGT || state == LEFT_OFFTGT_RIGHT_TGT ||
          state == LEFT_TGT_RIGHT_TGT) {
        setRight(LIGHTS_TGT);
      } else if (state == RIGHT_OFFTGT || state == LEFT_OFFTGT_RIGHT_OFFTGT ||
          state == LEFT_OFFTGT_RIGHT_TGT) {
        setRight(LIGHTS_OFFTGT);
      } else {
        setRight(LIGHTS_OFF);
      }

      if (micros() - timerMain >= REARM_TIME * 1000L) {
        state = WAIT;
      }
      break;
  }
}
