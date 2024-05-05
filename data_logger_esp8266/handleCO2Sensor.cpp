#include "handleCO2Sensor.h"

float getVoltage(int reading) {
  float r = reading;
  return 5.0 * (r / 1024.0);
}

int getReading(int sensorPin) {
  uint8_t i = 0;
  float buffer = 0;
  for (i = 0; i < 10; i++) {
    buffer += analogRead(sensorPin);
    delay(20); // 20ms
  }
  buffer /= i; // compute the mean
  return round(buffer);
}

int toPPM(float v) {
  float buffer = 0;
  float r400 = 645; // 820
  float r40000 = 494;

  float v400 = getVoltage(r400);
  float v40000 = getVoltage(r40000);

  float rV = v400 - v40000;
  float b = rV / log10(400.0 / 1000.0);
  float a = (v - v400) / b + log10(400.0);

  return round(pow(10, a));
}