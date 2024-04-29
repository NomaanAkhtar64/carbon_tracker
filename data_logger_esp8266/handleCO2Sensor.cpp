#include "handleCO2Sensor.h"

float getVoltage(int reading){
  float r = reading;
  return 5.0 * (r/1023.0);
}

int getReading(int sensorPin){
  uint8_t i = 0;
  float buffer = 0;
  for(i = 0; i < 10; i++){
      buffer += analogRead(sensorPin);
      delay(20); // 20ms
  }
  buffer /= i; // compute the mean
  return round(buffer);
}

int toPPM(float reading) {
  float buffer = 0;
  float r400 = 820; // 820
  float r40000 = 686;
  // Serial.println(reading);
  float v400 = getVoltage(r400);
  float v40000 = getVoltage(r40000);
  buffer = (v400 - v40000)/(log10(400) - log10(10000)); // Delta V
  buffer = (reading - v400)/buffer;
  buffer += log10(400);
  return round(pow(10, buffer));
}