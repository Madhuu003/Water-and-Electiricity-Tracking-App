/*********************************
   CLIENT ID:E1825
   DEVELOPER NAME:SARAVANAN
   DATE:03.08.2022
   STATUS:
 *********************************/
#include "lcd.h"
#include<SoftwareSerial.h>
SoftwareSerial ss(7, 6);
#include "current.h"
#include "voltage.h"
int count = 0;

bool leakage_flag = false;
/*** Dont change this value --value from datasheet**********************/
int sensorInterrupt = 0;  // interrupt 0
int sensorPin       = 2; //Digital Pin 2

unsigned int SetPoint = 400; //400 milileter
/*The hall-effect flow sensor outputs pulses per second per litre/minute of flow.*/
float calibrationFactor = 90; //You can change according to your datasheet
volatile byte pulseCount = 0;
float flowRate = 0.0;
unsigned int flowMilliLitres = 0;
float power_per_second = 0;
float avg_power_per_hour = 0, consumed_power = 0;
unsigned long totalMilliLitres = 0;
unsigned long totalMilliLitres1 = 0;

unsigned long oldTime = 0;
/*************************************************************************/
bool refill_flag = false;

void setup()
{
  Serial.begin(9600);
  ss.begin(9600);

  ac_current_init();
  ac_voltage_init();
  lcd.begin(16, 2);
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("HOUSE HOLD");
  lcd.setCursor(3, 1);
  lcd.print("MONITORING");
  delay(2000);
  lcd.clear();
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING); //you can use Rising or Falling
}

void loop()
{
  if ((millis() - oldTime) > 1000)   // Only process counters once per second
  {
    lcd.setCursor(0, 0);
    lcd.print("F:");
    // Disable the interrupt while calculating flow rate and sending the value to the host
    detachInterrupt(sensorInterrupt);
    // Because this loop may not complete in exactly 1 second intervals we calculate the number of milliseconds that have passed since the last execution and use that to scale the output. We also apply the calibrationFactor to scale the output based on the number of pulses per second per units of measure (litres/minute in this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    if (totalMilliLitres != 0)
    {
      totalMilliLitres1 = totalMilliLitres;
    }
    unsigned int frac;
    lcd.setCursor(2, 0);
    lcd.print(flowMilliLitres, DEC);
    ac_current();
    //ac_voltage();
    lcd.setCursor(0, 1);
    lcd.print("V:");
    lcd.setCursor(2, 1);
    lcd.print(supply_volt);
    lcd.setCursor(10, 1);
    lcd.print("I:");
    lcd.setCursor(12, 1);
    lcd.print(supply_current);
    //
    //    Serial.print("VOLTAGE:");
    //    Serial.println(supply_voltage);
    //    Serial.print("CURRENT:");
    //    Serial.println(supply_current);

    float Power = supply_volt * supply_current;

    //    Serial.print("POWER:");
    //    Serial.println(Power, 4);
    //
    //    Serial.println("P:" + String(Power));
    //    //  lcd.setCursor(0, 1);
    //  lcd.print("P:");
    //  lcd.setCursor(2, 1);
    //  lcd.print(Power);
    power_per_second = Power / 3600;
    //Serial.println("POWER_PER_SEC:" + String(power_per_second, DEC));
    consumed_power += power_per_second;
    //Serial.println(consumed_power, 3);
    lcd.setCursor(8, 0);
    lcd.print("CP:");
    lcd.setCursor(11, 0);
    lcd.print(consumed_power, 3);
    count++;
    if (count % 5 == 0)
    {
      send_app("$" + String(consumed_power) + "#" + "!" + String(totalMilliLitres) + "#");
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("DATA SEND-1");
      delay(2000);
      lcd.clear();

    }
    //delay(1000);
  }
  // Reset the pulse counter so we can start incrementing again
  Serial.println(count);
  pulseCount = 0;
  if (count == 30)
  {
    Serial.println("DATA SEND");
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("DATA SEND");
    send_app("*" + String(consumed_power) + "#" + "@" + String(totalMilliLitres1) + "#");
    delay(2000);
    lcd.clear();

    count = 0;
    consumed_power = 0;
    totalMilliLitres = 0;
  }
  // Enable the interrupt again now that we've finished sending output
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  delay(200);
}
//Insterrupt Service Routine

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}


void send_app(String s)
{
  for (int i = 0; i < s.length(); i++)
  {
    ss.write(s[i]);
  }
  delay(3000);
}
