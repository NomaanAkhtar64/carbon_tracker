# Carbon Tracker

This repo contains the complete software architecture for a realtime data logging system.

<img src="./docs/images/react.jpg" width="48px" height="32px">
<img src="./docs/images/django.jpg" width="32px" height="32px">
<img src="./docs/images/arduino.png" width="32px" height="32px">
<img src="./docs/images/esp32.jpg" width="32px" height="32px">
<img src="./docs/images/nodemcu.png" width="32px" height="32px">

---

### <ins>Packages</ins>

- #### Server
  The server uses django rest framework to create a REST Api. All the data is stored by the server
- #### Mobile
  The mobile app uses React Native to interface with the datalogging iot device in order to control and monitor the data flow.
- #### IoT Devices
  esp32 and esp8266 based devices to record and stream carbon concentration readings.

---

### <ins>Current Progress</ins>

| S.No | Project            | Progress          |
| ---- | ------------------ | ----------------- |
| 1    | Server             | Working Prototype |
| 2    | ESP8266 DataLogger | Working Prototype |
| 3    | ESP32 DataLogger   | Working Prototype |
| 4    | Mobile             | In Progress       |
| 5    | Web-App            | Coming Soon       |
