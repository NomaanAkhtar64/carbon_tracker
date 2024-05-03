#include "secret.h" // SECRET FILE WITH "ssid", "password", "serverURL", and "loginInfo"

///      CLOUD CONFIG      ///
#define loginURL serverURL "/login/"
#define addReadingURL serverURL "/api/readings/"

///      SD CONFIG      ///
#define SD_FILENAME "/data_log.csv"

// SAVE CONFIG
#define READING_SAVE_TIME_MIN 1

///      GPIO CONFIG      ///
#define CS_PIN 2
#define CHARGING_DETECTOR_PIN 33
#define BATTERY_PIN 32
#define PUSH_BTN_PIN 34
#define I2C_SDA_PIN 21 // I2C Data pin
#define I2C_SCL_PIN 22 // I2C Clock pin

///      DHT CONFIG      ///
#define DHT_PIN 14 // G14
#define DHT_TYPE DHT11
