#ifndef ACCURRENT_H
#define ACCURRENT_H
#include "ACS712.h"
/*
  This example shows how to measure the power consumption
  of devices in 230V electrical system
  or any other system with alternative current
*/

// We have 30 amps version sensor connected to A0 pin of arduino
// Replace with your version if necessary

//#define calibration 0.86

float I,supply_current,supply_volt;

ACS712 sensor(ACS712_20A, A0);

void ac_current_init() {
  // calibrate() method calibrates zero point of sensor,
  // It is not necessary, but may positively affect the accuracy
  // Ensure that no current flows through the sensor at this moment
  // If you are not sure that the current through the sensor will not leak during calibration - comment out this method
  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  sensor.calibrate();
  Serial.println("Done!");
}

void ac_current() {
  // We use 230V because it is the common standard in European countries
  // Change to your local, if necessary
  float U = 230;

  // To measure current we need to know the frequency of current
  // By default 50Hz is used, but you can specify desired frequency
  // as first argument to getCurrentAC() method, if necessary
   I = sensor.getCurrentAC();

  // To calculate the power we need voltage multiplied by current
  float P = U * I;
  //Serial.println(String("NOISE = ") + I + " A");
  if (I <= 0.25)
  {
    supply_current = 0;
  }
  else
  {
    supply_current = I;
    //supply_volt=random(230,240);
  }
  //Serial.println(String("I = ") + I + " A");
  // Serial.println(String("P = ") + P + " Watts");
  //return I;
}
#endif
