# Stepper-28BYJ-48
  This is a library for Arduino that can be used to control 28BYJ-48 stepper motor, commonly found on ebay or other places.

This library enables the user to set wanted position of the motor in steps or degrees, and the user can decide will he use blocking 
or non blocking method. The motor can rotate with constant speed, or the speed of the motor can be changed overtime with accerelation.

The speed of the motor can be set from 1 Rpm to 15 Rpm, and accerelation can be set from 10 Rpm^2 to 400 Rpm^2. My suggestion is to use 100
Rmp^2 or 200 Rpm^2 as it gives the finest speed/moment relation.

To install this library, just download the .zip and extract it to Arduino/libraries folder
