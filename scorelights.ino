#include <Adafruit_CircuitPlayground.h>

#include "include/constants.h"
#include "include/scorelights.h"

int currentR[10];
int currentG[10];
int currentB[10];

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
}
