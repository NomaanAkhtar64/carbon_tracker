#include "device.h"
#include "WiFi.h"
#include "device_config.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <MHZ19.h>
#include <NTPClient.h>
#include <RTClib.h>
#include <SD.h>
#include <TimeLib.h>
#include <Wire.h>
#include <wifi.h>

long timer1;

WiFiClient wclient;
HTTPClient httpClient;
JsonDocument doc;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 18000, 60000);
MHZ19 mhz19b;
HardwareSerial hardwareSerial(2);
DHT dht(DHT_PIN, DHT_TYPE);

void DataLogger::setup() {
  pinMode(CHARGING_DETECTOR_PIN, INPUT);
  pinMode(BATTERY_PIN, INPUT);
  setupSensors();

  setupI2C();
  screen.setup();
  screen.displayLogo();
  delay(2000);
  setupWiFi();
  delay(1000);
  setupSD();
  delay(1000);
  setupRTC();
  delay(1000);

  if (isConnectedToWifi.get()) {
    loginToCloud();
    delay(1000);
  }

  calibrateSensors();
  getSensorReadings();
  getDisplayTime();
  setupRTCAlarms();

  screen.drawMainScreen(ppmCO2.get(), humidity.get(), surroudingTemp.get(),
                        isBatteryCharging.get(), isConnectedToWifi.get(),
                        batteryPercentage.get(), displayDateTimeStr);
}

void DataLogger::update() {
  if (digitalRead(PUSH_BTN_PIN)) {
    Serial.println("EVENT: BUTTON PRESSED");
    if (screen.isAsleep) {
      screen.wake();
    } else {
      screen.sleep();
    }
  }
  Serial.println(digitalRead(PUSH_BTN_PIN));
  if (rtc.lostPower()) {
    Serial.println("RTC has Lost Power!");
  }
  synchronizeRTC();

  if (millis() - timer1 > 500) {
    getSensorReadings();
    screen.updateMainScreen(&ppmCO2, &humidity, &surroudingTemp,
                            &isBatteryCharging, &batteryPercentage,
                            &isConnectedToWifi);
    timer1 = millis();
  }
  if (rtc.alarmFired(1) == true) {
    DateTime now = rtc.now();
    getDataString();
    saveToSD();
    saveToCloud();

    ppmCO2.clear();
    rawCO2.clear();
    surroudingTemp.clear();
    humidity.clear();

    rtc.clearAlarm(1);

    rtc.setAlarm1(rtc.getAlarm1() + TimeSpan(0, 0, 1, 0), DS3231_A1_Minute);
  }

  if (rtc.alarmFired(2) == true) {
    DateTime now = rtc.now();

    screen.updateDateTime(displayDateTimeStr);
    rtc.clearAlarm(2);
    rtc.setAlarm1(rtc.getAlarm1() + TimeSpan(0, 0, 1, 0), DS3231_A1_Minute);
  }
}

void DataLogger::setupSD() {
  pinMode(CS_PIN, OUTPUT);
  if (!SD.begin(CS_PIN)) {
    screen.printError("SD Card", "Initialization", "Failed!");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  isSDWorking = true;
  screen.printStatus("SD Card", "Ready", "SUCCESS");
}

void DataLogger::setupWiFi() {
  screen.printStatus("Connecting to", "Wifi", "STATUS");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
  }

  for (int i = 0; i < 15; i++) // Try to connect for 3 seconds
  {
    delay(200);
    if (WiFi.status() == WL_CONNECTED) {
      break;
    };
  }

  if (WiFi.status() == WL_CONNECTED) {
    isConnectedToWifi.set(true);
    screen.printStatus("Connected to", "WiFi", "SUCCESS");
  } else {
    isConnectedToWifi.set(false);
    screen.printError("Could not", "connect to", "WiFi");
  }

  WiFi.onEvent(
      [this](WiFiEvent_t event, WiFiEventInfo_t info) {
        isConnectedToWifi.set(true);
        Serial.print("Connected To WiFi");
      },
      WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

  WiFi.onEvent(
      [this](WiFiEvent_t event, WiFiEventInfo_t info) {
        isConnectedToWifi.set(false);
        Serial.print("WiFi lost connection. Reason: ");
        Serial.println(info.wifi_sta_disconnected.reason);
      },
      WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
}

void DataLogger::setupI2C() {
  if (!Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN)) {
    Serial.println("ERROR: Failed to configure I2C bus");
    return;
  }
  isI2CWorking = true;
}

void DataLogger::setupRTC() {
  if (!isI2CWorking)
    return;

  if (!rtc.begin(&Wire)) {
    screen.printError("DS3231", "Initialization", "Failed!");
    return;
  }
  isRTCWorking = true;

  screen.printStatus("Synchronizing", "Clock", "STATUS");
  timeClient.begin();
  synchronizeRTC();
  screen.printStatus("Synchronized", "Clock", "SUCCESS");

  // Wire.beginTransmission(0x68); // address DS3231
  // Wire.write(0x0E);             // select CONTROL register
  // Wire.write(0b000011100);      // write register (bit 7 is EOSC)
  // Wire.endTransmission();
  // Wire.beginTransmission(0x68);
  // Wire.write(0x0F);       // select STATUS register
  // Wire.write(0b00001000); // write register (bit 7 is OSF)
  // Wire.endTransmission();
}

void DataLogger::setupSensors() {
  hardwareSerial.begin(9600);
  mhz19b.begin(hardwareSerial);
  dht.begin();
}

void DataLogger::calibrateSensors() { mhz19b.autoCalibration(); }

void DataLogger::loginToCloud() {
  httpClient.begin(wclient, loginURL);
  httpClient.addHeader("Content-Type", "application/json");

  int httpResponseCode = httpClient.POST(loginInfo);
  String payload = "{}";

  char responeCodeStr[8];
  sprintf(responeCodeStr, "%d", httpResponseCode);

  if (httpResponseCode != 200) {
    screen.printError("Failed to", "Login Response", responeCodeStr);
    httpClient.end();
    return;
  }

  payload = httpClient.getString();
  deserializeJson(doc, payload);
  if (doc.isNull()) {
    screen.printError("Failed to", " Login!", "Null");

    httpClient.end();
    return;
  }
  if (!doc.containsKey("token")) {
    screen.printError("Failed to", " Login!", "Token Key error");

    httpClient.end();
    return;
  }

  isLoggedIN = true;

  String tokenBuffer = doc["token"];
  token = "token " + tokenBuffer;
  httpClient.end();
  screen.printStatus("Logged In", "to Cloud", "SUCCESS");
}

void DataLogger::saveToCloud() {
  if (!isLoggedIN) {
    loginToCloud();
  }
  if (!isLoggedIN)
    return;

  JsonDocument reading;
  reading["time_stamp"] = dataDateTimeStr;
  reading["temperature"] = surroudingTemp.getAverage();
  reading["humidity"] = (int)round(humidity.getAverage());
  reading["raw_reading"] = (int)round(rawCO2.getAverage());
  reading["co2_ppm"] = (int)round(ppmCO2.getAverage());
  reading["dataset"] = 3;

  String readingJSONString = "";
  serializeJson(reading, readingJSONString);
  Serial.println(readingJSONString);

  httpClient.begin(wclient, addReadingURL);
  httpClient.addHeader("Content-Type", "application/json");
  httpClient.addHeader("Authorization", token);
  int httpResponseCode = httpClient.POST(readingJSONString);

  if (httpResponseCode != 201) {
    Serial.println("Did not get Response 201");
    Serial.println(httpClient.getString());
    httpClient.end();
    return;
  }
  Serial.println("Sent data to cloud!");
  httpClient.end();
}

void DataLogger::getDisplayTime() {
  strncpy(displayDateTimeStr, "hh:mm DD/MM/YY", sizeof(displayDateTimeStr));
  DateTime rtcTime = rtc.now();
  rtcTime.toString(displayDateTimeStr);
  Serial.println(displayDateTimeStr);
}

void DataLogger::setupRTCAlarms() {
  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  DateTime currentTime = rtc.now();
  int secondsTillNextMin = 60 - currentTime.second();
  DateTime nextSaveTime = currentTime + TimeSpan(secondsTillNextMin + 60);
  //   int nextSaveMin = ceil((rtcTime.minute() * 60 + rtcTime.second()) /
  //                          (60.0f * READING_SAVE_TIME_MIN));
  //   int nextScreenTimeMin = rtcTime.minute() + 1;
  rtc.setAlarm1(nextSaveTime, DS3231_A1_Minute);
  rtc.setAlarm2(nextSaveTime, DS3231_A2_Minute);
}

void DataLogger::synchronizeRTC() {
  if (!isRTCWorking && WiFi.status() != WL_CONNECTED)
    return;

  if (timeClient.update()) {
    unsigned long unix_epoch = timeClient.getEpochTime();
    rtc.adjust(DateTime(unix_epoch));
  }
}
void DataLogger::getDataString() {
  char buffer[] = "YYYY-MM-DD hh:mm:ss";
  DateTime rtcTime = rtc.now();
  strncpy(buffer, "YYYY-MM-DD hh:mm:ss", sizeof(buffer));
  rtcTime.toString(buffer);
  strncpy(dataDateTimeStr, buffer, sizeof(buffer));
  sprintf(dataString, "%s,%.2f,%.2f,%.2f,%.2f", dataDateTimeStr,
          ppmCO2.getAverage(), rawCO2.getAverage(), surroudingTemp.getAverage(),
          humidity.getAverage());
}

void DataLogger::saveToSD() {
  File sensorDataFile;
  if (!SD.exists(SD_FILENAME)) {
    Serial.print("File not Found!   Creating........");
    Serial.println(SD_FILENAME);
    sensorDataFile = SD.open(SD_FILENAME, FILE_WRITE);
  } else {
    sensorDataFile = SD.open(SD_FILENAME, FILE_APPEND);
  }

  if (sensorDataFile) {
    sensorDataFile.println(dataString);
    sensorDataFile.close();

    Serial.print("SAVED TO SD: ");
    Serial.println(dataString);
  } else {
    Serial.println("Failed to save to SD Card! ");
  }
}

int interpolate(float x2, float x, float x1, int y2, int y1) {
  return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
}

int DataLogger::getBatteryPercentage() {
  // 100	4.2
  // 90	4.12
  // 80	4.04
  // 70	3.96
  // 60	3.88
  // 50	3.8
  // 40	3.68
  // 30	3.56
  // 20	3.4
  // 10	3.2
  // 0	3.0
  int aReading = analogRead(BATTERY_PIN);
  double Vo = 3.3 * (aReading / 4095.0);
  int R2 = 33000; // 33K
  int R1 = 15000; // 15K

  double batteryVoltage = (Vo * (R1 + R2)) / R2;
  Serial.print("Battery Voltage: ");
  Serial.println(batteryVoltage);

  if (batteryVoltage > 4.12f && batteryVoltage <= 4.20f) {
    return interpolate(4.20f, batteryVoltage, 4.12f, 100, 90);
  } else if (batteryVoltage > 4.04f && batteryVoltage <= 4.12f) {
    return interpolate(4.12f, batteryVoltage, 4.04f, 90, 80);
  } else if (batteryVoltage > 3.96f && batteryVoltage <= 4.04f) {
    return interpolate(4.04f, batteryVoltage, 3.96f, 80, 70);
  } else if (batteryVoltage > 3.88f && batteryVoltage <= 3.96f) {
    return interpolate(3.96f, batteryVoltage, 3.88f, 70, 60);
  } else if (batteryVoltage > 3.8f && batteryVoltage <= 3.88f) {
    return interpolate(3.88f, batteryVoltage, 3.8f, 60, 50);
  } else if (batteryVoltage > 3.68f && batteryVoltage <= 3.8f) {
    return interpolate(3.8f, batteryVoltage, 3.68f, 50, 40);
  } else if (batteryVoltage > 3.56f && batteryVoltage <= 3.68f) {
    return interpolate(3.68f, batteryVoltage, 3.56f, 40, 30);
  } else if (batteryVoltage > 3.4f && batteryVoltage <= 3.56f) {
    return interpolate(3.56f, batteryVoltage, 3.4f, 30, 20);
  } else if (batteryVoltage > 3.2f && batteryVoltage <= 3.4f) {
    return interpolate(3.4f, batteryVoltage, 3.2f, 20, 10);
  } else if (batteryVoltage > 3.0f && batteryVoltage <= 3.2f) {
    return interpolate(3.2f, batteryVoltage, 3.0f, 10, 0);
  }

  return 0;
}

void DataLogger::getSensorReadings() {
  isBatteryCharging.set(digitalRead(CHARGING_DETECTOR_PIN));
  batteryPercentage.set(getBatteryPercentage());
  rawCO2.set(mhz19b.getCO2Raw());
  ppmCO2.set(mhz19b.getCO2());
  sensorTemp.set(mhz19b.getTemperature());
  humidity.set(dht.readHumidity());
  surroudingTemp.set(dht.readTemperature());

  if (isnan(humidity.get()) || isnan(surroudingTemp.get())) {
    screen.printError("Temperature", "Sensor", "Failure");
  }
}