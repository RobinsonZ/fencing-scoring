#include "include/sabre.h"

#include <Arduino.h>

#include "include/constants.h"
#include "include/scorelights.h"

#define WAIT 1
#define LEFT_WAIT 2
#define RIGHT_WAIT 3

// all the targets must be contiguous
#define LEFT_TOUCH 4
#define RIGHT_TOUCH 5
#define DOUBLE_TOUCH 6

unsigned long sabreTimerMain;
unsigned int sabreState = WAIT;
unsigned int sabreLastState = 0;

void sabre_setup() {
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

void setErrLeft(bool body, bool weapon) {
  if (weapon) {
    // technically not compliant with spec, which calls for a WHITE light and
    // audible signal, and a yellow light with no signal for grounding.
    setLeft(LIGHTS_OFFTGT);
  } else if (body) {
    setLeft(LIGHTS_GND);
  } else {
    setLeft(LIGHTS_OFF);
  }
}

void setErrRight(bool body, bool weapon) {
  if (weapon) {
    // technically not compliant with spec, which calls for a WHITE light and
    // audible signal, and a yellow light with no signal for grounding.
    setRight(LIGHTS_OFFTGT);
  } else if (body) {
    setRight(LIGHTS_GND);
  } else {
    setRight(LIGHTS_OFF);
  }
}

void setErrLights(bool left_body, bool left_weapon, bool right_body,
                  bool right_weapon) {
  setErrLeft(left_body, left_weapon);
  setErrRight(right_body, right_weapon);
}

void sabre_state() {
  /*
  here, we determine which things are grounded to what by separately pulling
  each B line LOW. I couldn't find exact numbers but the electric signal
  propagates through copper cable at about 0.65c which means it'll cover the
  ground in well under 1 microsecond. The write and read methods take several
  microseconds each so we'll be fine in terms of propagation delay.
  */

  // also since the minimum touch time in sabre is 100 microseconds we won't bother with a
  // pre-touch state


  pinMode(LEFT_B, OUTPUT);
  pinMode(RIGHT_B, INPUT);
  // pull left LOW, right HIGH
  digitalWrite(LEFT_B, LOW);
  bool left_contact = !digitalRead(RIGHT_A);
  bool left_body_contact = !digitalRead(LEFT_A);
  bool left_weapon_ok = !digitalRead(LEFT_C);

  pinMode(RIGHT_B, OUTPUT);
  pinMode(LEFT_B, INPUT);
  digitalWrite(RIGHT_B, LOW);
  bool right_contact = !digitalRead(LEFT_A);
  bool right_body_contact = !digitalRead(RIGHT_A);
  bool right_weapon_ok = !digitalRead(RIGHT_C);

  if (sabreState != sabreLastState) {
    Serial.println(sabreState);
    sabreLastState = sabreState;
  }

  switch (sabreState) {
    case WAIT:
      setErrLights(left_body_contact, !left_weapon_ok, right_body_contact, !right_weapon_ok);

      if (left_contact) {
        sabreTimerMain = micros();
        sabreState = LEFT_WAIT;
      } else if (right_contact) {
        sabreTimerMain = micros();
        sabreState = RIGHT_WAIT;
      }  // otherwise stay in WAIT
      break;
    case LEFT_WAIT:
      setLeft(LIGHTS_TGT);
      setErrRight(right_body_contact, !right_weapon_ok);

      if (right_contact) {
        sabreTimerMain = micros();
        sabreState = DOUBLE_TOUCH;
      } else if (micros() - sabreTimerMain >= SABRE_DOUBLE_PERIOD * 1000L) {
        sabreTimerMain = micros();
        sabreState = LEFT_TOUCH;
      }  // otherwise stay here
      break;
    case RIGHT_WAIT:
      setRight(LIGHTS_TGT);
      setErrLeft(left_body_contact, !left_weapon_ok);

      if (left_contact) {
        sabreTimerMain = micros();
        sabreState = DOUBLE_TOUCH;
      } else if (micros() - sabreTimerMain >= SABRE_DOUBLE_PERIOD * 1000L) {
        sabreTimerMain = micros();
        sabreState = RIGHT_TOUCH;
      }  // otherwise stay here
      break;
    case LEFT_TOUCH:
      setLeft(LIGHTS_TGT);
      setErrRight(right_body_contact, !right_weapon_ok);

      if (micros() - sabreTimerMain >= REARM_TIME * 1000L) {
        sabreState = WAIT;
      }
      break;
    case RIGHT_TOUCH:
      setRight(LIGHTS_TGT);
      setErrLeft(left_body_contact, !left_weapon_ok);

      if (micros() - sabreTimerMain >= REARM_TIME * 1000L) {
        sabreState = WAIT;
      }
      break;
    case DOUBLE_TOUCH:
      setLeft(LIGHTS_TGT);
      setRight(LIGHTS_TGT);

      if (micros() - sabreTimerMain >= REARM_TIME * 1000L) {
        sabreState = WAIT;
      }
      break;
  }
}
