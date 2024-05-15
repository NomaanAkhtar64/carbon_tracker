
#include "dataLogger.h"
#include <arduino-timer.h>

DataLogger device;

auto timer = timer_create_default();
#define DisplayUpdateTime 300 // ms + 200ms
#define SaveRate 1 // min

bool getReadingAndUpdateLCD(void *) { return device.getReadingAndUpdateLCD(); }
bool saveReadingsToCloudAndSD(void *) {
  return device.saveReadingsToCloudAndSD();
}
void setup() {
  Serial.begin(9600);
  device.beginLCD();
  delay(1000);
  device.setupMUX();
  device.connectToWifi();
  delay(1000);
  device.setupSD();
  delay(1000);
  device.setupRTC();
  delay(1000);
  device.loginToCloud();
  delay(1000);
  timer.every(DisplayUpdateTime, getReadingAndUpdateLCD);
  timer.every(SaveRate * 60000, saveReadingsToCloudAndSD);
}

void loop() {
  timer.tick();
  device.listenForPushButton();
}