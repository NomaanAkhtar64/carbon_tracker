#include "oled.h"
#include <RTClib.h>

class DataLogger {
  OLED screen;
  RTC_DS3231 rtc;

  bool isSDWorking = false;
  bool isLoggedIN = false;
  bool isI2CWorking = false;
  bool isRTCWorking = false;

  DataVarRAvg rawCO2;
  DataVarRAvg ppmCO2;
  DataVarRAvg humidity;
  DataVarRAvg surroudingTemp;

  String token;
  char displayDateTimeStr[14];
  char dataString[140];
  char dataDateTimeStr[20];

  DataVar<float> sensorTemp = DataVar<float>(0.0);
  DataVar<bool> isBatteryCharging = DataVar<bool>(false);
  DataVar<int> batteryPercentage = DataVar<int>(0);
  DataVar<bool> isConnectedToWifi = DataVar<bool>(false);

  void setupSensors();
  void setupSD();
  void setupI2C();
  void setupRTC();
  void setupWiFi();
  void loginToCloud();

  void synchronizeRTC();
  void setupRTCAlarms();
  void calibrateSensors();

  int getBatteryPercentage();
  void getDisplayTime();
  void getDataString();
  void saveToCloud();
  void saveToSD();

public:
  void getSensorReadings();

  void setup();
  void update();
};