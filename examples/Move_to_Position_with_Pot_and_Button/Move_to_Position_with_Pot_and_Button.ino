#include "Stepper-28BYJ-48.h"
Stepper motor; // default pins are 8,9,10,11
void setup()
{
  pinMode(3, INPUT);
  motor.setRPM(15); // RPM can be set from 1RPM to 15RPM
  motor.setAccerelation(200); // Accerelation can be set from 10 to 400
}
uint32_t ltime = 0;
uint32_t btime=0;
bool lstate = false;
uint32_t t = 0;
void loop()
{
  // if the motor starts to jerk, change the accerelation
  motor.move();
  if (micros() - ltime >= 150000) {
    if (digitalRead(3) != lstate && lstate==false && micros()-btime>=1000) {
      btime=micros();
      lstate == true;
      ltime = micros();
      int val = analogRead(2);
      val = map(val, 0, 1023, 0, 359);
      motor.setDegree(val);
    }
    else lstate = digitalRead(3);
  }
}
