#ifndef Scorelights_h
#define Scorelights_h

#define LIGHTS_OFF 0
#define LIGHTS_TGT 1
#define LIGHTS_OFFTGT 2
#define LIGHTS_GND 3

extern void initBlink();
extern void setLeft(int setting);
extern void setRight(int setting);

#endif