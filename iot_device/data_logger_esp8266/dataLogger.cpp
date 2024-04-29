#include "datalogger.h"

// I2C_PINS
#define SDA_PIN D2
#define SCL_PIN D1

// MUX_PINS
#define MUX_PIN_DELAY 20 // ms
#define MUX_A D0
#define MUX_B D3

// utility function for switching MUX
void switchMUX(int B, int A) {
  digitalWrite(MUX_B, B);
  digitalWrite(MUX_A, A);
  delay(MUX_PIN_DELAY);
}

// Wifi Credentials
char ssid[] = "Maaz-home";
char password[] = "34915311";

// SD_CARD PINS
#define CS_PIN D8

// Cloud
String serverURL = "http://192.168.1.164:8000";
#define loginInfo                                                              \
  "{\"username\": \"data_logger_1\",\"password\": "                            \
  "\"C5j85Az$QLP9ZJFr3u%nFM*b\"}"
// PUSH_BUTTON_PIN
#define BUTTON_PIN A0

// SENSOR_PINS
#define CO2_PIN A0
#define DHT11_PIN D4

// loopTimes
#define DisplayUpdateTime 300 // ms + 200ms
#define SaveRate 5 // min

DataLogger::DataLogger() {}

void DataLogger::printLCD(String upperline, String lowerline) {
  lcd.home();
  lcd.clear();
  lcd.println(upperline);
  lcd.setCursor(0, 1);
  lcd.println(lowerline);
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

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
  }
  lcd.clear();
}

void DataLogger::setupSD() {
  pinMode(CS_PIN, OUTPUT);
  printLCD("   Insert  SD   ", "      Card      ");
  if (!SD.begin(CS_PIN)) {
    printLCD("    SD Failed   ", "                ");
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

void DataLogger::loginToCloud() {
  WiFiClient client;
  HTTPClient http;
  String loginURL = serverURL + "/login/";
  http.begin(client, loginURL.c_str());
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
  JSONVar responseJSON = JSON.parse(payload);

  if (JSON.typeof(responseJSON) == "undefined") {
    printLCD("Failed to Login!", "                ");
    Serial.println("Did not get JSON Response");

    http.end();
    return;
  }

  if (!responseJSON.hasOwnProperty("token")) {
    printLCD("Failed to Login!", "                ");
    Serial.println("No token property in JSON!");

    http.end();
    return;
  }

  token = (String)responseJSON["token"];
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
  const String fileName = "data_log.csv";
  if (!SD.exists(fileName)) {
    printLCD("File not found !", "  Creating....  ");
    Serial.println("File not found!");
    Serial.print("Creating...");
    Serial.println(fileName);
  }
  sensorData = SD.open(fileName, FILE_WRITE);
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

  JSONVar reading;
  reading["time_stamp"] = timeStamp;
  reading["temperature"] = (float)tempSum / readings;
  reading["humidity"] = (int)humSum / readings;
  reading["raw_reading"] = (int)co2RawSum / readings;
  reading["co2_ppm"] = (int)co2Sum / readings;
  reading["data_logger"] = 1;

  String readingJSONString = JSON.stringify(reading);
  Serial.println(readingJSONString);
  WiFiClient client;
  HTTPClient http;
  String addReadingURL = serverURL + "/api/readings/";
  http.begin(client, addReadingURL.c_str());
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "token " + token);
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
    saveToSD();
    saveToCloud(buffer);
    co2Sum = 0;
    tempSum = 0;
    co2RawSum = 0;
    humSum = 0;
    readings = 0;
  }
  return true;
}