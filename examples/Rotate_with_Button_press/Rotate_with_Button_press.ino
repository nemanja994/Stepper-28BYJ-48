#include "Stepper-28BYJ-48.h"
Stepper motor; // default pins are 8,9,10,11
int right=3;
int left=4;
void setup()
{
  pinMode(left,INPUT);
  pinMode(right,INPUT);
  motor.setRPM(15); // RPM can be set from 1RPM to 15RPM
  motor.stop();
}
int dir=0;
void loop()
{
  if(digitalRead(right)==HIGH) dir=1;
  else if(digitalRead(left)==HIGH) dir=2;
  switch(dir){
    case 1: motor.rotateCW();
    break;
    case 2: motor.rotateCCW();
    break;
    default: motor.stop();
    }
}
