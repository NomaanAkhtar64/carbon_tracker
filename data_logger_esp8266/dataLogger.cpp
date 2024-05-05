#include "datalogger.h"
#include "secret.h"

#define SD_FILENAME "./data.csv"

// I2C_PINS
#define SDA_PIN D2
#define SCL_PIN D1

// MUX_PINS
#define MUX_PIN_DELAY 20 // ms
#define MUX_A D0
#define MUX_B D3
bool isSDWorking = true;

// utility function for switching MUX
void switchMUX(int B, int A) {
  digitalWrite(MUX_B, B);
  digitalWrite(MUX_A, A);
  delay(MUX_PIN_DELAY);
}

// SD_CARD PINS
#define CS_PIN D8

// PUSH_BUTTON_PIN
#define BUTTON_PIN A0

// SENSOR_PINS
#define CO2_PIN A0
#define DHT11_PIN D4

// loopTimes
#define DisplayUpdateTime 300 // ms + 200ms
#define SaveRate 5 // min
WiFiClient client;
HTTPClient http;

DataLogger::DataLogger() {}

void DataLogger::printLCD(String upperline, String lowerline) {
  lcd.home();
  lcd.clear();
  lcd.println(upperline);
  lcd.setCursor(0, 1);
  lcd.println(lowerline);
  Serial.print(upperline);
  Serial.print(" ");
  Serial.println(lowerline);
}

void DataLogger::beginLCD() {
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.begin(16, 2);
  printLCD("    Starting    ", "   DataLogger   ");
}

void DataLogger::setupMUX() {
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);
  switchMUXDevice(CO2_SENSOR);
}

void DataLogger::switchMUXDevice(MUX_DEVICES device) {
  switch (device) {
  case CO2_SENSOR:
    return switchMUX(LOW, HIGH); // 01 = 1
  case PUSH_BUTTON:
    return switchMUX(LOW, LOW); // 00 = 0
  default:
    return;
  };
}

void DataLogger::connectToWifi() {
  lcd.clear();
  lcd.println("Wifi Connecting");
  lcd.setCursor(0, 1);
  WiFi.begin(ssid, password);

  for (int i = 0; i < 6; i++) // Try to connect for 3 seconds
  {
    lcd.print(".");
    delay(500);
    if (WiFi.status() == WL_CONNECTED) {
      break;
    };
  }

  if (WiFi.status() != WL_CONNECTED) {
    printLCD("   Could  not   ", "connect to  Wifi");
  }

  lcd.clear();
}

void DataLogger::setupSD() {
  pinMode(CS_PIN, OUTPUT);
  // printLCD("   Insert  SD   ", "      Card      ");
  if (!SD.begin(CS_PIN)) {
    isSDWorking = false;
    return printLCD("    SD Failed   ", "                ");
  }
  printLCD("    SD Ready    ", "                ");
}

void DataLogger::setupRTC() {
  lcd.clear();
  printLCD("  Synchronizing ", "      Clock     ");
  rtc.begin();
  timeClient.begin();
  synchronizeRTC();
}

void DataLogger::synchronizeRTC() {
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    unsigned long unix_epoch = timeClient.getEpochTime();
    rtc.adjust(DateTime(unix_epoch));
  }
}
JsonDocument doc;

void DataLogger::loginToCloud() {
  http.begin(client, loginURL);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(loginInfo);
  String payload = "{}";
  Serial.println(httpResponseCode);

  if (httpResponseCode != 200) {
    printLCD("Failed to Login!", "                ");
    Serial.println("Did not get Response 200");

    http.end();
    return;
  }

  payload = http.getString();
  payload = http.getString();
  deserializeJson(doc, payload);

  if (doc.isNull()) {
    printLCD("Failed to Login!", "                ");
    Serial.println("Did not get JSON Response; got null");

    http.end();
    return;
  }

  if (!doc.containsKey("token")) {
    printLCD("Failed to Login!", "                ");
    Serial.println("No token property in JSON!");

    http.end();
    return;
  }

  String tokenBuffer = doc["token"];
  sprintf(token, "%s", tokenBuffer);
  isLoggedIN = true;

  Serial.println("Logged In to Cloud!");
  printLCD("  Logged  into  ", "     Cloud!     ");
}

void DataLogger::listenForPushButton() {
  switchMUXDevice(PUSH_BUTTON);
  bool pushed = analogRead(BUTTON_PIN) == 1024;
  if (pushed && lastState == false) {
    isScreenON = !isScreenON;
  }
  lastState = pushed;
}

bool DataLogger::getReadingAndUpdateLCD() {
  switchMUXDevice(CO2_SENSOR);
  int rawVal = getReading(CO2_PIN); // 200ms to get 10 stable readings
  Serial.println(rawVal);
  int val = toPPM(getVoltage(rawVal));
  DHT.read(DHT11_PIN);
  co2Sum += val;
  co2RawSum += rawVal;
  tempSum += DHT.temperature;
  humSum += DHT.humidity;
  readings += 1;

  if (isScreenON) {
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CO2:");
    lcd.print(val);
    lcd.print(" ppm ");
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(DHT.temperature);
    lcd.print("C  h:");
    lcd.print(DHT.humidity);
    lcd.print("%");
  } else {
    lcd.clear();
    lcd.noBacklight();
  }

  return true;
}

void DataLogger::saveToSD() {
  if (!SD.exists(SD_FILENAME)) {
    printLCD("File not found !", "  Creating....  ");
    Serial.println("File not found!");
    Serial.print("Creating ");
    Serial.println(SD_FILENAME);
  }
  sensorData = SD.open(SD_FILENAME, FILE_WRITE);
  if (sensorData) {
    sensorData.println(dataString);
    sensorData.close();
    Serial.println("saved to SD");
  } else {
    lcd.clear();
    printLCD(" Failed to save ", "   data to SD   ");
    Serial.println("Failed to save data to SD Card!");
  }
}

void DataLogger::saveToCloud(char timeStamp[19]) {
  if (!isLoggedIN) {
    loginToCloud();
  }
  if (!isLoggedIN)
    return;

  JsonDocument reading;
  reading["time_stamp"] = timeStamp;
  reading["temperature"] = (float)(tempSum / readings);
  reading["humidity"] = (int)round(humSum / readings);
  reading["raw_reading"] = (int)round(co2RawSum / readings);
  reading["co2_ppm"] = (int)round(co2Sum / readings);
  reading["dataset"] = 4;

  String readingJSONString = "";
  serializeJson(reading, readingJSONString);
  Serial.println(readingJSONString);

  char tokenStr[46];
  sprintf(tokenStr, "token %s", token);

  http.begin(client, addReadingURL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", tokenStr);
  int httpResponseCode = http.POST(readingJSONString);

  if (httpResponseCode != 201) {
    printLCD(" Failed to send ", " data to cloud! ");
    Serial.println("Did not get Response 201");
    Serial.println(http.getString());
    http.end();
    return;
  }
  Serial.println("Sent data to cloud!");
  http.end();
}

bool DataLogger::saveReadingsToCloudAndSD() {
  if (readings > 0) {
    synchronizeRTC();
    DateTime rtcTime = rtc.now();
    char buffer[19];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", rtcTime.year(),
            rtcTime.month(), rtcTime.day(), rtcTime.hour(), rtcTime.minute(),
            rtcTime.second());
    sprintf(dataString, "%s,%d,%d,%d,%d", buffer, co2Sum / readings,
            co2RawSum / readings, tempSum / readings, humSum / readings);
    if (isSDWorking) saveToSD();
    if (WiFi.status() == WL_CONNECTED) saveToCloud(buffer);
    co2Sum = 0;
    tempSum = 0;
    co2RawSum = 0;
    humSum = 0;
    readings = 0;
  }
  return true;
}