#include "oled.h"
// #include <Adafruit_GFX.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <cmath>

/// ICONS
const uint8_t batteryChargingIcon[] = { // 20x20
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7c, 0xcf, 0x00, 0xfc, 0xcf, 0x80, 0xc1, 0x81, 0x80, 0xc3, 0x81, 0x80,
    0xc3, 0x01, 0xb0, 0xc7, 0xf1, 0xb0, 0xc7, 0xf1, 0xb0, 0xc0, 0x61, 0x90,
    0xc0, 0xc1, 0x80, 0xc1, 0xc1, 0x80, 0xf9, 0x9f, 0x80, 0x79, 0x1f, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t batteryIcon[] = { // 20x20
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7f, 0xff, 0x00, 0xff, 0xff, 0x80, 0xc0, 0x01, 0x80, 0xc0, 0x01, 0x80,
    0xc0, 0x01, 0xb0, 0xc0, 0x01, 0xb0, 0xc0, 0x01, 0xb0, 0xc0, 0x01, 0xb0,
    0xc0, 0x01, 0x80, 0xc0, 0x01, 0x80, 0xff, 0xff, 0x80, 0x7f, 0xff, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t cloudIcon[] = { // 24x24
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf8, 0x00,
    0x1f, 0xfe, 0x00, 0x3f, 0x7f, 0x00, 0x7c, 0x0f, 0x80, 0x78, 0x07, 0x80,
    0xf0, 0x03, 0xc0, 0xe0, 0x01, 0xf8, 0xe0, 0x01, 0xfe, 0xe0, 0x00, 0x3e,
    0xe0, 0x00, 0x07, 0xe0, 0x00, 0x07, 0xe0, 0x00, 0x07, 0xf0, 0x00, 0x07,
    0xf0, 0x00, 0x07, 0x7c, 0x00, 0x07, 0x3f, 0x80, 0x3e, 0x1f, 0xff, 0xfe,
    0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t bluetoothIcon[] = { // 12x12
    0x06, 0x00, 0x07, 0x00, 0x27, 0x80, 0x16, 0xc0, 0x0f, 0x00, 0x06, 0x00,
    0x0e, 0x00, 0x1f, 0x00, 0x16, 0xc0, 0x27, 0x80, 0x07, 0x00, 0x06, 0x00};
const uint8_t wifiIcon[] = { // 12x14
    0x00, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x7f, 0xe0, 0xe0, 0x70,
    0xdf, 0xb0, 0x3f, 0xc0, 0x30, 0xc0, 0x0f, 0x00, 0x1f, 0x80,
    0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x00, 0x00};
const uint8_t thermometerIcon[] = { // 18x18
    0x01, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x03, 0x60, 0x00, 0x03, 0x20,
    0x00, 0x03, 0x20, 0x00, 0x03, 0x20, 0x00, 0x03, 0x20, 0x00, 0x03,
    0x20, 0x00, 0x03, 0x20, 0x00, 0x03, 0x20, 0x00, 0x03, 0x20, 0x00,
    0x06, 0x30, 0x00, 0x06, 0x18, 0x00, 0x0c, 0x18, 0x00, 0x0c, 0x18,
    0x00, 0x06, 0x38, 0x00, 0x07, 0xf0, 0x00, 0x01, 0xe0, 0x00};
const uint8_t umbrellaIcon[] = { // 16x16
    0x01, 0x80, 0x0f, 0xf0, 0x3f, 0xfc, 0x78, 0x1e, 0x70, 0x0e, 0xe0,
    0x07, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xff, 0x01, 0x80, 0x01, 0x80,
    0x01, 0x80, 0x01, 0x98, 0x01, 0xf8, 0x01, 0xf8, 0x00, 0xf0};

void OLED::setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  };
}

void OLED::printStatus(char *line1, char *line2, char *status) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 26);
  display.setTextWrap(0);
  display.setCursor((128 - strlen(line1) * 6) / 2, 26);
  display.println(line1);
  display.setTextWrap(0);
  display.setCursor((128 - strlen(line2) * 6) / 2, 34);
  display.println(line2);
  display.setCursor(0, 0);
  display.setTextWrap(0);
  display.setCursor(43, 0);
  display.println(status);
  display.drawLine(0, 0, 40, 0, 1);
  display.drawLine(85, 0, 128, 0, 1);
  display.drawLine(0, 0, 0, 64, 1);
  display.drawLine(127, 0, 127, 64, 1);
  display.drawLine(0, 63, 128, 63, 1);
  display.display();

  Serial.printf("%s: %s %s\n", status, line1, line2);
}

void OLED::printError(char *line1, char *line2, char *line3) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 24);
  display.setTextWrap(0);

  display.setCursor((128 - strlen(line1) * 6) / 2, 24);
  display.println(line1);
  display.setTextWrap(0);

  display.setCursor((128 - strlen(line2) * 6) / 2, 32);
  display.println(line2);
  display.setTextWrap(0);

  display.setCursor((128 - strlen(line3) * 6) / 2, 40);
  display.println(line3);
  display.setCursor(0, 0);

  display.setTextWrap(0);
  display.setCursor(49, 0);
  display.println("ERROR");
  display.drawLine(0, 0, 45, 0, 1);
  display.drawLine(80, 0, 128, 0, 1);
  display.drawLine(0, 0, 0, 64, 1);
  display.drawLine(127, 0, 127, 64, 1);
  display.drawLine(0, 63, 128, 63, 1);
  display.display();

  Serial.printf("ERROR: %s %s %s\n", line1, line2, line3);
}

void OLED::displayLogo() {
  display.clearDisplay();

  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println("Carbon");
  display.println("Tracker");

  display.display();
}

void OLED::sleep() {
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  isAsleep = true;
}

void OLED::wake() {
  display.ssd1306_command(SSD1306_DISPLAYON);
  isAsleep = false;
}

void OLED::drawMainScreen(int ppmCO2, int humidity, int surroudingTemp,
                          bool isBatteryCharging, bool isConnectedToWiFi,
                          int batteryPercentage, char *dateTimeStr) {

  char batteryPercentageBuffer[10];
  char ppmBuffer[10];
  char humidityBuffer[10];

  // Serial.println(ppm)
  sprintf(ppmBuffer, "%4d", min(ppmCO2, 9999));
  sprintf(humidityBuffer, "%3d", min(humidity, 99));
  sprintf(batteryPercentageBuffer, "%02d", min(batteryPercentage, 99));

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  display.setCursor(2, 3);
  display.print(dateTimeStr);

  if (isBatteryCharging)
    display.drawBitmap(107, -3, batteryChargingIcon, 20, 20, 1);
  else {
    display.drawBitmap(107, -3, batteryIcon, 20, 20, 1);
    display.setCursor(110, 4);
    display.print(batteryPercentageBuffer);
  }

  display.setCursor(105, 32);
  display.print("ppm");

  display.setTextSize(3);
  display.setCursor(33, 18);
  display.print(ppmBuffer);

  display.fillRoundRect(28, 45, 4, 4, 4, 1);
  display.setTextSize(2);
  display.setCursor(5, 46);
  display.print(surroudingTemp);
  display.setCursor(32, 46);
  display.print("C");
  display.setCursor(62, 46);
  display.print(humidityBuffer);
  display.setCursor(98, 46);
  display.print("%");

  display.setTextSize(1);
  display.drawLine(0, 0, -0, 64, 1);
  display.drawLine(127, 0, 127, 64, 1);
  display.drawLine(0, 63, 128, 63, 1);
  display.drawLine(0, 14, 128, 14, 1);
  display.drawLine(0, 0, 128, 0, 1);
  display.drawLine(0, 42, 128, 42, 1);
  display.drawLine(59, 42, 59, 64, 1);

  display.drawBitmap(3, 15, cloudIcon, 24, 24, 1);

  // display.drawBitmap(96, 1, bluetoothIcon, 12, 12, 1);

  if (isConnectedToWiFi)
    display.drawBitmap(89, 0, wifiIcon, 12, 14, 1);

  display.drawBitmap(43, 44, thermometerIcon, 18, 18, 1);
  display.drawBitmap(110, 45, umbrellaIcon, 16, 16, 1);

  display.display();
}

void OLED::updateMainScreen(DataVarRAvg *ppmCO2, DataVarRAvg *humidity,
                            DataVarRAvg *surroudingTemp,
                            DataVar<bool> *isBatteryCharging,
                            DataVar<int> *batteryPercentage,
                            DataVar<bool> *isConnectedToWifi) {
  bool changed = false;

  char batteryPercentageBuffer[10];
  char ppmBuffer[10];
  char humidityBuffer[10];

  if (ppmCO2->hasChanged()) {
    changed = true;
    sprintf(ppmBuffer, "%4d", min(ppmCO2->get(), 9999));
    display.setTextSize(3);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(33, 18);
    display.print(ppmBuffer);
  }

  if (humidity->hasChanged()) {
    changed = true;
    display.setTextSize(2);
    display.setTextColor(WHITE, BLACK);
    sprintf(humidityBuffer, "%3d", min(humidity->get(), 99));
    display.setCursor(62, 46);
    display.print(humidityBuffer);
  }

  if (surroudingTemp->hasChanged()) {
    changed = true;
    display.setTextSize(2);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(5, 46);
    display.print(surroudingTemp->get());
  }

  if (isBatteryCharging->hasChanged()) {
    changed = true;
    display.setTextColor(WHITE, BLACK);

    display.fillRect(107, -3, 20, 20, 0);
    display.drawBitmap(
        107, -3, isBatteryCharging->get() ? batteryChargingIcon : batteryIcon,
        20, 20, 1);
  }

  if (batteryPercentage->hasChanged() && !isBatteryCharging->get()) {
    changed = true;
    sprintf(batteryPercentageBuffer, "%02d", min(batteryPercentage->get(), 99));
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.drawBitmap(107, -3, batteryChargingIcon, 20, 20, 1);
    display.drawBitmap(107, -3, batteryIcon, 20, 20, 1);
    display.setCursor(110, 4);
    display.print(batteryPercentageBuffer);
  }

  if (changed)
    display.display();
}

void OLED::updateDateTime(char *dateTimeStr) {

  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);

  display.setCursor(2, 3);
  display.print(dateTimeStr);

  display.display();
}
