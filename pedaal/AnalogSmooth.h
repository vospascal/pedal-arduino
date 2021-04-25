/*
  AnalogSmooth.h - Library to smooth analog signal 
  jitter by averaging concurrent readings.
  Created by Michael Thessel.
  https://github.com/MichaelThessel/arduino-analog-smooth/blob/master/AnalogSmooth.h
*/
 
#ifndef AnalogSmooth_h
#define AnalogSmooth_h

#include "Arduino.h"

class AnalogSmooth
{
	public:
		AnalogSmooth(unsigned int windowSize);
		AnalogSmooth();
		float analogReadSmooth(uint8_t pin);
		float smooth(float value);
	private:
		unsigned int _windowSize;
		uint8_t _pin;
		float _analog[100];
		unsigned int _analogPointer;
		unsigned int _maxPointer;
		void _init(unsigned int windowSize);
};
#endif
