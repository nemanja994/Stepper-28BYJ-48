#include "Stepper-28BYJ-48.h"

// -----------------------------------------------------------------
// Class Constructor
// -----------------------------------------------------------------

Stepper::Stepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4){
	// This is the Class constructor. It set's all the class variables to default values and sets pins as OUTPUT
	this->Speed.mspeed = 0;
	this->Speed.interval = 0;
	this->Speed.accerelation = 0;
	this->Speed.fn = 0;
	this->Position.current_position = 0;
	this->Position.target_position = 0;
	this->Position.last_step = 0;
	this->pins[0] = pin1;
	this->pins[1] = pin2;
	this->pins[2] = pin3;
	this->pins[3] = pin4;
	for (int8_t pin = 0; pin < 4; ++pin){
		pinMode(this->pins[pin], OUTPUT);
		digitalWrite(this->pins[pin], LOW);
	}
}

// -----------------------------------------------------------------
// Class methods
// -----------------------------------------------------------------

void Stepper::setRPM(uint8_t dspeed){
	/* This method is used to set the speed of the motor in RPM (revolutions per minute)
	The actual speed of the motor can vary from the wanted one slightly, because the interval is integer
	Resolution of arduino clock is small, micros() only return number that is multiple of 4, so there is
	no use of putting interval as float*/
	if (dspeed>15) {
		//Serial.println("Maximum speed of this motor is 15RPM");
		dspeed = 15;
	}
	else if (dspeed < 1){
		//Serial.println("Minimum speed of this motor is 1RPM");
		dspeed = 1;
	}
	// the interval is calculated in microsecond for increased precision
	this->Speed.mspeed = dspeed;
	this->Speed.interval = floor(1000000 * (float(60) / (this->Speed.mspeed * this->Number_of_Steps)));
}

void Stepper::setAccerelation(uint8_t accerelation){
	// this method is used to set Accerelation of the motor, and calculate needed details afterwards
	if (accerelation > 400){
		//Serial.println("Maximum accerelation of this motor is 400RPM^2");
		accerelation = 400;
	}
	else if (accerelation < 10){
		//Serial.println("Minimum accerelation of this motor is 10RPM^2");
		accerelation = 10;
	}
	if (this->Speed.accerelation == 0){ // If the accerelation is being set for the first time
		this->Speed.accerelation = accerelation;
		//Serial.println(this->Speed.n);
	}
	else {
		this->Speed.fn = (this->Speed.fn*this->Speed.accerelation / accerelation);
		this->Speed.accerelation = accerelation;
	}
}

bool Stepper::isSpeedReached(uint16_t interval){
	// this method checks if the wanted speed is reached
	if (this->Speed.interval< interval) return false;
	else return true;
}

uint16_t Stepper::calculateDelay(){
	// This method is used to recalculate the interval with given accerelation
	if (this->Speed.accerelation > 0){ 
		if (this->Speed.fn == 0){
			float alfa = 2 * PI / this->Number_of_Steps;
			this->Speed.c = 0.676*sqrt(2 * alfa / (this->Speed.accerelation)*9.55);
			return (0);
		}
		if (!isSpeedReached(1000 * this->Speed.c *this->Speed.interval) && distanceToGo()>0.15*this->Position.distance){
			this->Speed.c -= (2 * this->Speed.c) / (4 * distanceToGo() + 1);
			return (1000 * this->Speed.c*this->Speed.interval);
		}
		if (distanceToGo() <= 0.15*this->Position.distance){
			this->Speed.c += (2 * this->Speed.c) / (4 * this->Speed.fn + 1);
			return (1000 * this->Speed.c*this->Speed.interval);
		}
		else {
			return this->Speed.interval;
		}
	}
	else { // if accerelation wasn't set, just return the wanted interval
		return this->Speed.interval;
	}
}

void Stepper::stop(){
	// This method is used to stop the motor, by pulling the pins LOW
	for (int8_t i = 0; i < 4; ++i){
		digitalWrite(pins[i], LOW);
	}
}

void Stepper::step(uint8_t combination){
	// this method is used to send impulses to the motor according to the current step
	for (int8_t i = 0; i < 4; ++i){
		digitalWrite(pins[i], sequence[combination][i]);
	}
}

void Stepper::rotateCCW(){
	// This function can be used for driving the motor continiously in CCW direction
	uint32_t now = micros();
	if (abs(now - this->Position.last_step) >= this->Speed.interval){
		this->Position.last_step = now;
		step(Position.current_position % 8);
		if (Position.current_position == 0) this->Position.current_position = this->Number_of_Steps;
		else --this->Position.current_position;
	}
}

void Stepper::rotateCW(){
	// This function can be used for driving the motor continiously in CW direction
	uint32_t now = micros();
	if (abs(now - this->Position.last_step) >= this->Speed.interval){
		this->Position.last_step = now;
		step(Position.current_position % 8);
		if (Position.current_position == this->Number_of_Steps) this->Position.current_position = 0;
		else ++this->Position.current_position;
	}
}

void Stepper::setNewPosition(uint16_t position){
	/* This method is used to set new target position of the motor. It accepts numbers ranging from -4076 to 4076
	-4076 is one full rotation backwards, and 4076 is one full rotation forwards. If the user enters value larger/smaller than those
	the method will automatically recalculate target_position*/
	if (position >= this->Number_of_Steps) position -= this->Number_of_Steps*(floor(position / 4076));
	this->Position.target_position = position;
	if (position > this->Position.current_position) Position.direction = Stepper::CW;
	else Position.direction = Stepper::CCW;
	int16_t tmp = this->Position.target_position;
	tmp = this->Position.current_position - tmp;
	this->Position.distance = abs(tmp);
}

void Stepper::setDegree(uint16_t deg){
	if (deg >= 360) deg -= 360 * floor(deg / 360);
	float tmp = float(deg) / (float(360) / this->Number_of_Steps);
	setNewPosition(tmp);
}

uint16_t Stepper::distanceToGo(){
	// this method calculates how many steps does motor need to go before it reaches the target position
	return  this->Position.distance - this->Speed.fn;
}

void Stepper::move(){
	// This method is called in every iteration of loop, and it doesn't block the rest of the board
	if (this->Speed.interval>0){
		uint16_t steps_left = distanceToGo();
		if (steps_left > 0){
			uint32_t now = micros();
			if ((now - this->Position.last_step) >= calculateDelay()){
				this->Position.last_step = now;
				if (Position.direction == Stepper::CW){
					step(this->Position.current_position % 8);
					if (Position.current_position == this->Number_of_Steps) this->Position.current_position = 0;
					else ++this->Position.current_position;
				}
				else {
					step(this->Position.current_position % 8);
					if (Position.current_position == 0) this->Position.current_position = this->Number_of_Steps;
					else --this->Position.current_position;
				}
				this->Speed.fn++;
			}
		}
		else {
			stop();
			this->Speed.fn = 0;
			this->Position.distance = 0;
		}
	}
	//else Serial.println("Can't move, the speed of the motor wasn't set");
}

void Stepper::moveWithBlock(){
	// This method is used to set motor to target position while blocking the rest of the board
	if (this->Speed.interval){
		uint16_t steps_left = distanceToGo();
		while (steps_left > 0){
			uint32_t now = micros();
			if ((now - this->Position.last_step) >= calculateDelay()){
				this->Position.last_step = now;
				if (Position.direction == Stepper::CW){
					step(this->Position.current_position % 8);
					if (Position.current_position == this->Number_of_Steps) this->Position.current_position = 0;
					else ++this->Position.current_position;
				}
				else {
					step(this->Position.current_position % 8);
					if (Position.current_position == 0) this->Position.current_position = this->Number_of_Steps;
					else --this->Position.current_position;
				}
				this->Speed.fn++;
				--steps_left;
			}
		}
		stop();
		this->Speed.fn = 0;
		this->Position.distance = 0;
	}
	//else Serial.println("Can't move, the speed of the motor wasn't set");
}

bool Stepper::isRunning(){
	if(this->Position.distance==0 || this->Speed.fn==0) return false;
	else return true;
	//(this->Position.distance==0 || this->Speed.fn==0) ? false: true;
}