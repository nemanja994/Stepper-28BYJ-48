#include "Stepper-28BYJ-48.h"
Stepper motor; // default pins are 8,9,10,11
void setup()
{
  motor.setRPM(15); // RPM can be set from 1RPM to 15RPM
}
uint16_t timer;
void loop()
{
  // rotate in CW direction for 10 seconds than rotate in CCW 
  // direction
  if(millis()<=1000*10) motor.rotateCW();
  else motor.rotateCCW();
}
