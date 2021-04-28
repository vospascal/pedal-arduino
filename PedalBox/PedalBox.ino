// if arduino doest upload wire RST to GND to reset and upload empty sketch

#include "Pedals.h"

Pedals pedals;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  pedals.setThrottleBits("12bit", "15bit");
  pedals.setThrottleAnalogPin(A0);
//  //pedals.setThrottleADSChannel(1);
//  //pedals.setThrottleLoadCell(6,5);
//
  pedals.setBrakeBits("12bit", "16bit");
  pedals.setBrakeAnalogPin(A3);
//  //pedals.setBrakeADSChannel(1);
//  //pedals.setBrakeLoadcell(7,5);
//
  pedals.setClutchBits("12bit", "10bit");
  pedals.setClutchAnalogPin(A1);
//  //pedals.setClutchADSChannel(1);
//  //pedals.setClutchLoadcell(8,5);
//
  pedals.setup();

  //  delay(2000);
}

// the loop routine runs over and over again forever:
void loop() {
  //  timing
//  unsigned long start = micros();

  pedals.loop();

//  unsigned long end = micros();
//  unsigned long delta = end - start;
//  Serial.println(delta);
}
