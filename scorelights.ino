#include <Adafruit_CircuitPlayground.h>
#include <TimerOne.h>

#include "include/constants.h"
#include "include/scorelights.h"

// gamma correction, see
// https://learn.adafruit.com/led-tricks-gamma-correction/the-quick-fix

const uint8_t PROGMEM gamma8[] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,
    2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,
    4,   5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,   8,
    8,   9,   9,   9,   10,  10,  10,  11,  11,  11,  12,  12,  13,  13,  13,
    14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,
    21,  22,  22,  23,  24,  24,  25,  25,  26,  27,  27,  28,  29,  29,  30,
    31,  32,  32,  33,  34,  35,  35,  36,  37,  38,  39,  39,  40,  41,  42,
    43,  44,  45,  46,  47,  48,  49,  50,  50,  51,  52,  54,  55,  56,  57,
    58,  59,  60,  61,  62,  63,  64,  66,  67,  68,  69,  70,  72,  73,  74,
    75,  77,  78,  79,  81,  82,  83,  85,  86,  87,  89,  90,  92,  93,  95,
    96,  98,  99,  101, 102, 104, 105, 107, 109, 110, 112, 114, 115, 117, 119,
    120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142, 144, 146,
    148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175, 177,
    180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252,
    255};

int currentR[10];
int currentG[10];
int currentB[10];

volatile int left;
volatile int right;
bool blink;

void doBlink() {
  switch (left) {
    case LIGHTS_OFF:
      digitalWrite(LEFT_TARGET, LOW);
      break;
    case LIGHTS_GND:
      digitalWrite(RIGHT_TARGET, LOW);
      break;
    case LIGHTS_OFFTGT:
      digitalWrite(LEFT_TARGET, blink ? HIGH : LOW);
      break;
    case LIGHTS_TGT:
      digitalWrite(LEFT_TARGET, HIGH);
  }
  switch (right) {
    case LIGHTS_OFF:
      digitalWrite(RIGHT_TARGET, LOW);
      break;
    case LIGHTS_GND:
      digitalWrite(RIGHT_TARGET, LOW);
      break;
    case LIGHTS_OFFTGT:
      digitalWrite(RIGHT_TARGET, blink ? HIGH : LOW);
      break;
    case LIGHTS_TGT:
      digitalWrite(RIGHT_TARGET, HIGH);
  }
  blink = !blink;
}
void initBlink() {
  Timer1.initialize(100000);
  Timer1.attachInterrupt(doBlink);
  Timer1.start();
}

void setLights(int from, int to, int r, int g, int b, bool doTone) {
  bool diff = false;
  for (int i = from; i < to; i++) {
    if (currentR[i] != r || currentG[i] != g || currentB[i] != b) {
      CircuitPlayground.setPixelColor(i, pgm_read_byte(&gamma8[r]),
                                      pgm_read_byte(&gamma8[g]),
                                      pgm_read_byte(&gamma8[b]));
      currentR[i] = r;
      currentG[i] = g;
      currentB[i] = b;
      diff = true;
    }
  }
  if (diff && doTone) {
    tone(5, 2000, 750);
  }
}

void setLeft(int setting) {
  int r, g, b;
  bool tone;
  switch (setting) {
    case LIGHTS_OFF:
      r = g = b = 0;
      tone = false;
      break;
    case LIGHTS_OFFTGT:
      r = 255;
      g = 255;
      b = 0;
      tone = true;
      break;
    case LIGHTS_TGT:
      r = 255;
      g = 0;
      b = 0;
      tone = true;
      break;
    case LIGHTS_GND:
      r = 122;
      g = 120;
      b = 65;
      tone = false;
  }

  setLights(0, 5, r, g, b, tone);

  left = setting;
}

void setRight(int setting) {
  int r, g, b;
  bool tone;
  switch (setting) {
    case LIGHTS_OFF:
      r = g = b = 0;
      tone = false;
      break;
    case LIGHTS_OFFTGT:
      r = 255;
      g = 255;
      b = 0;
      tone = true;
      break;
    case LIGHTS_TGT:
      r = 0;
      g = 255;
      b = 0;
      tone = true;
      break;
    case LIGHTS_GND:
      r = 122;
      g = 120;
      b = 65;
      tone = false;
  }

  setLights(5, 10, r, g, b, tone);

  right = setting;
}
