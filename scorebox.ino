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

#include <Adafruit_CircuitPlayground.h>

#include "include/constants.h"
#include "include/foil.h"

// settings (for now)
#define MODE MODE_FOIL

void setup() {
  CircuitPlayground.begin();
  Serial.begin(115200);
  while (!Serial)
    ;

  pinMode(LEFT_TARGET, OUTPUT);
  pinMode(LEFT_OFF_TARGET, OUTPUT);

  switch (MODE) {
    case MODE_FOIL:
      foil_setup();
      break;
    case MODE_EPEE:
      // TODO
      break;
    case MODE_SABRE:
      // TOOD
      break;
  }
}

void loop() {
  switch (MODE) {
    case MODE_FOIL:
      foil_state();
      break;
    case MODE_EPEE:
      // TODO
      break;
    case MODE_SABRE:
      // TOOD
      break;
  }
}
