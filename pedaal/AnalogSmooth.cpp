/*
  AnalogSmooth.cpp - Library to smooth analog signal 
  jitter by averaging concurrent readings.
  Created by Michael Thessel.
  https://github.com/MichaelThessel/arduino-analog-smooth/blob/master/AnalogSmooth.ccp
*/
#include "Arduino.h"
#include "AnalogSmooth.h"

/*
  Constructor
*/
AnalogSmooth::AnalogSmooth()
{
	this->_init(10);
}

/*
  Constructor
*/
AnalogSmooth::AnalogSmooth(unsigned int windowSize)
{
	this->_init(windowSize);
}

/*
  Initialzie the environment
*/
void AnalogSmooth::_init(unsigned int windowSize)
{
	// Restrict the size of the history array 
	// to >= 1 and < 100 items
	if (windowSize > 100) { windowSize = 100; }
	if (windowSize < 1) { windowSize = 1; }
	this->_windowSize = windowSize;
	
	this->_analogPointer = 0;
	this->_maxPointer = 0;
}

/*
  Perform smooting of analog input from given value 
*/
float AnalogSmooth::smooth(float value)
{
	// Return if we only keep track of 1 value
	if (this->_windowSize == 1) {
		return value;
	}

	// Save the value to the history array	
	this->_analog[this->_analogPointer] = value;
	
	// Calculate the moving average
	float total = 0;
	for (int i = 0; i <= this->_maxPointer; i++) {
		total = total + this->_analog[i];
	}
	float avg = total / (this->_maxPointer + 1);
	  
	// Keep track of how many items we have in the array
	if (this->_maxPointer < this->_windowSize - 1) {
		this->_maxPointer++;
	}
	 
	// Update the array pointer 
	this->_analogPointer++;
	if (this->_analogPointer == this->_windowSize) {
		this->_analogPointer = 0;
	}
	  
	// Retrun the average
	return avg;
}

/*
  Perform smooting of analog input from given pin 
*/
float AnalogSmooth::analogReadSmooth(uint8_t pin)
{
	// Read the pin
	float current = analogRead(pin);
	
	return this->smooth(current);
}
