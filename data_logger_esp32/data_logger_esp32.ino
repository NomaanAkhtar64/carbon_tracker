

#include "device.h"

DataLogger device;

void setup() {

  Serial.begin(115200);
  while (!Serial) {
  };
  device.setup();
}

void loop() { device.update(); }