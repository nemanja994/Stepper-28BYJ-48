// Stepper-28BYJ-48.h
/* This library is used for driving the 28BYJ-48 stepper motor. It can drive the motor
without blocking the rest of the board, or it can be used with blocking functions. This
library also supports accerelation/decerelation of the motor. The user can select
speed in RPM or in pulses per second */

#ifndef _STEPPER-28BYJ-48_h
#define _STEPPER-28BYJ-48_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Stepper{
private:
	enum Direction{
		CW, // Clockwise
		CCW, // Counterclockwise
	};
	const bool sequence[8][4] = // each row represents combination of impulses for turning on the coils
	{
		{ HIGH, LOW, LOW, LOW },
		{ HIGH, HIGH, LOW, LOW },
		{ LOW, HIGH, LOW, LOW },
		{ LOW, HIGH, HIGH, LOW },
		{ LOW, LOW, HIGH, LOW },
		{ LOW, LOW, HIGH, HIGH },
		{ LOW, LOW, LOW, HIGH },
		{ HIGH, LOW, LOW, HIGH },
	};
	// Class variables
	uint8_t pins[4]; // digital output pins
	struct SpeedC{  // speed cluster
		uint8_t mspeed; // minimum speed is 1RPM and maximum 15RPM
		uint16_t interval; // delay betweeen two steps, this delay actually represents the speed of the motor
		float c; // delay multiplier
		uint8_t accerelation;
		uint16_t fn; // variable which counts how many times was calculateDelay() function called
	} Speed;
	struct PositionC{ // position cluster
		Direction direction;
		uint16_t current_position;
		uint16_t distance;
		uint32_t last_step; // time in microseconds, when the last step was performed
		uint16_t target_position;
	} Position;
	static const uint16_t Number_of_Steps = 4076; //number of steps in one full rotation
	// Private methods
	void step(uint8_t);
	uint16_t calculateDelay();
	uint16_t distanceToGo();
	bool isSpeedReached(uint16_t);
public:
	// Class constructor
	Stepper(const uint8_t pin1 = 8, const uint8_t pin2 = 9, const uint8_t pin3 = 10, const uint8_t pin4 = 11);
	// Public class methods
	void setRPM(uint8_t); // maximum speed is 15 RPM
	void setAccerelation(uint8_t);
	void rotateCW();
	void rotateCCW();
	void setNewPosition(uint16_t);
	void setDegree(uint16_t);
	void move();
	void moveWithBlock();
	void stop();
	bool isRunning();
};

#endif