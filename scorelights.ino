#include <Adafruit_CircuitPlayground.h>
#include <TimerOne.h>

#include "include/constants.h"
#include "include/scorelights.h"

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

void setLights(int from, int to, int r, int g, int b) {
  bool diff = false;
  for (int i = from; i < to; i++) {
    if (currentR[i] != r || currentG[i] != g || currentB[i] != b) {
      CircuitPlayground.setPixelColor(i, r, g, b);
      currentR[i] = r;
      currentG[i] = g;
      currentB[i] = b;
      diff = true;
    }
  }
  if (diff && (r || g || b)) {
    tone(5, 2000, 750);
  }
}

void setLeft(int setting) {
  int r, g, b;
  switch (setting) {
    case LIGHTS_OFF:
      r = g = b = 0;
      break;
    case LIGHTS_OFFTGT:
      r = 255;
      g = 255;
      b = 0;
      break;
    case LIGHTS_TGT:
      r = 255;
      g = 0;
      b = 0;
  }

  setLights(0, 5, r, g, b);

  left = setting;
}

void setRight(int setting) {
  int r, g, b;
  switch (setting) {
    case LIGHTS_OFF:
      r = g = b = 0;
      break;
    case LIGHTS_OFFTGT:
      r = 255;
      g = 255;
      b = 0;
      break;
    case LIGHTS_TGT:
      r = 0;
      g = 255;
      b = 0;
  }

  setLights(5, 10, r, g, b);

  right = setting;
}
