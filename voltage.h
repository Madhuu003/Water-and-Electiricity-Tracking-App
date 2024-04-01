#ifndef ACVOLTAGE_H
#define ACVOLTAGE_H
#include "EmonLib.h"             // Include Emon Library

#define VOLT_CAL 189 //1084

float V, supply_voltage;

EnergyMonitor emon1;             // Create an instance

void ac_voltage_init()
{
  //Serial.begin(9600);
  emon1.voltage(2, VOLT_CAL, 1.7);  // Voltage: input pin, calibration, phase_shift
}

void ac_voltage()
{
  emon1.calcVI(25, 1000);        // Calculate all. No.of half wavelengths (crossings), time-out
  V   = emon1.Vrms;             //extract Vrms into Variable

  if (V >= 170)
  {
    supply_voltage = V;
  }
  else
  {
    //supply_voltage = 0;
    //Serial.println("Power OFF");
  }
  //Serial.print("Voltage : ");
  //Serial.println(supply_voltage);
}
#endif
