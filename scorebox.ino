/*
Fencing Scoring Machine Code
Zachary Robinson
ENGR 015: Fudamentals of Digital and Embedded Systems (Fall 2021)
Swarthmore College

This code implements a fencing scoring machine according to the timing
guidelines in the USA Fencing Rules for Competition, Appendix B
("Characteristics of Scoring Apparatus"). See pages 178-183,
https://cdn2.sportngin.com/attachments/document/f840-2248253/2020-08_USA_Fencing_Rules.pdf

Implementation notes:

  * In foil, there's a 300ms (±25ms) period after the first touch where a second
    touch can still register, but the rules are somewhat unclear on whether that
    300ms starts at the moment the circuit breaks or when the touch is actually
    registered ~14 ms later. For ease of implementation I assume that the 300ms
    starts *after* the first 14ms.
*/

#include <TimerOne.h>

#include "include/constants.h"

// settings (for now)
#define MODE MODE_FOIL

// common globals
volatile int l_touch;  // 0: null, 1: on-target, 2: off-target
volatile int r_touch;  // 0: null, 1: on-target, 2: off-target

// starts counting up once a touch is scored
int double_timer = 0;

volatile bool can_score = true;
volatile int rearm_timer = 0;

// foil-specific globals
/**
 * whether the left foil is in pre-touch (waiting for 14ms)
 */
volatile bool lfoil_pretouch;
int lfoil_ticks = 0;

/**
 * whether the right foil is in pre-touch (waiting for 14ms)
 */
volatile bool rfoil_pretouch;
int rfoil_ticks = 0;


void foil_timing_side(volatile bool *pretouch, volatile int *touch, int *ticks,
                      int b_line, int a_line) {
  if (*pretouch) {
    if (digitalRead(b_line)) {  // button no longer pressed
      *pretouch = false;
      *ticks = 0;
    } else {
      *ticks++;

      if (*ticks > FOIL_MIN_TOUCH) {
        *touch = digitalRead(a_line) ? 2 : 1;

        *pretouch = false;
        *ticks = 0;
      }
    }
  }
}

void foil_timing_tick() {
  if (can_score) {
    foil_timing_side(&lfoil_pretouch, &l_touch, &lfoil_ticks, LEFT_B,
                     RIGHT_A);
    foil_timing_side(&rfoil_pretouch, &r_touch, &rfoil_ticks, RIGHT_B,
                     LEFT_A);

    if (l_touch || r_touch) {
      double_timer++;
      if (double_timer > FOIL_DOUBLE_PERIOD) {
        can_score = false;
        double_timer = 0;
      }
    }
  } else {
    rearm_timer++;
    if (rearm_timer > REARM_TIME) {
      lfoil_pretouch = 0;
      l_touch = 0;
      lfoil_ticks = 0;

      rfoil_pretouch = 0;
      r_touch = 0;
      rfoil_ticks = 0;

      double_timer = 0;
      rearm_timer = 0;

      can_score = true;
    }
  }
}

void foil_loop_tick() {
  // pins will be pulled LOW if there is a short between them and LEFT_B
  bool left_c = !digitalRead(LEFT_C);
  bool right_c = !digitalRead(RIGHT_C);

  // process each side separately

  if (can_score) {
    if (left_c && !(lfoil_pretouch || l_touch)) {
      // the timer will check back in 14 ms to see whether it's on-target
      lfoil_pretouch = true;
    }

    if (right_c && !(rfoil_pretouch || r_touch)) {
      rfoil_pretouch = true;
    }
  }
}

void do_lighting() {
  digitalWrite(LEFT_TARGET, l_touch == 1);
  digitalWrite(LEFT_OFF_TARGET, l_touch == 2);

  digitalWrite(RIGHT_TARGET, r_touch == 1);
  digitalWrite(RIGHT_OFF_TARGET, r_touch == 2);
}

void setup() {
  // set a 1ms timer (we have plus or minus at least 1ms on everything so)
  Timer1.initialize(1000);

  switch (MODE) {
    case MODE_FOIL:
      // we need to test connections between:
      // LEFT_B/LEFT_C
      // LEFT_B/RIGHT_A
      // RIGHT_B/RIGHT_C
      // RIGHT_B/LEFT_A

      // See this for method of setting inputs/outputs to check connection
      // https://forum.arduino.cc/t/detecting-input-pins-connected-to-each-other/20206/3

      // outputs
      pinMode(LEFT_B, OUTPUT);
      pinMode(RIGHT_B, OUTPUT);

      // inputs
      pinMode(LEFT_A, INPUT_PULLUP);
      pinMode(LEFT_C, INPUT_PULLUP);
      pinMode(RIGHT_A, INPUT_PULLUP);
      pinMode(RIGHT_B, INPUT_PULLUP);

      digitalWrite(LEFT_B, LOW);
      digitalWrite(RIGHT_B, LOW);

      Timer1.attachInterrupt(foil_timing_tick);
      break;
    case MODE_EPEE:
      // TODO
      break;
    case MODE_SABRE:
      // TODO
      break;
  }
}

void loop() {
  switch (MODE) {
    case MODE_FOIL:
      foil_loop_tick();
      break;
    case MODE_EPEE:
      // TODO
      break;
    case MODE_SABRE:
      // TOOD
      break;
  }

  do_lighting();
}
