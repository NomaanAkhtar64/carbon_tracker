#include "dataVariable.h"
#include <Adafruit_SSD1306.h>

///      LED CONFIG      ///
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

class OLED {
private:
  Adafruit_SSD1306 display =
      Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

public:
  bool isAsleep = true;
  void setup();
  void printError(char *line1, char *line2, char *line3);
  void printStatus(char *line1, char *line2, char *status);
  void displayLogo();
  void sleep();
  void wake();
  void drawMainScreen(int ppmCO2, int humidity, int surroudingTemp,
                      bool isBatteryCharging, bool isConnectedToWiFi,
                      int batteryPercentage, char *dateTimeStr);
  void updateMainScreen(DataVarRAvg *ppmCO2, DataVarRAvg *humidity,
                        DataVarRAvg *surroudingTemp,
                        DataVar<bool> *isBatteryCharging,
                        DataVar<int> *batteryPercentage,
                        DataVar<bool> *isConnectedToWifi);
  void updateDateTime(char *dateTimeStr);
};
