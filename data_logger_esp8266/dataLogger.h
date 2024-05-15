#include "handleCO2Sensor.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DFRobot_DHT11.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <pins_arduino.h>

enum MUX_DEVICES {
  CO2_SENSOR,
  PUSH_BUTTON,
};

class DataLogger {
private:
  // mux
  void switchMUXDevice(MUX_DEVICES device);
  // lcd
  LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  // rtc
  RTC_DS3231 rtc;
  WiFiUDP ntpUDP;
  NTPClient timeClient = NTPClient(ntpUDP, "pool.ntp.org", 18000, 60000);
  // login
  bool isLoggedIN;
  // push_button
  bool isScreenON = true;
  bool lastState = false;
  // get_readings
  DFRobot_DHT11 DHT;
  double readings = 0;
  int co2Sum = 0;
  int co2RawSum = 0;
  int tempSum = 0;
  int humSum = 0;
  // save_reading
  File sensorData;
  char dataString[100];
  void saveToSD();
  void saveToCloud(char timestamp[19]);

public:
  DataLogger();
  void beginLCD();
  void setupMUX();
  void setupSD();
  void setupRTC();
  void synchronizeRTC();
  void connectToWifi();
  void loginToCloud();
  void listenForPushButton();
  void printLCD(String upperline, String lowerline);
  bool getReadingAndUpdateLCD();
  bool saveReadingsToCloudAndSD();
};