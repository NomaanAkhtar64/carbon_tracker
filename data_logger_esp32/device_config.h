///      WIFI CONFIG      ///
char ssid[] = "Storm F1";
char password[] = "34915311";

///      CLOUD CONFIG      ///
String serverURL = "http://192.168.1.164:8000";
#define loginInfo                                                              \
  "{\"username\": \"data_logger_2\",\"password\": "                            \
  "\"Nq#r!ydon#2ARgs4FJrvWik6\"}"

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
