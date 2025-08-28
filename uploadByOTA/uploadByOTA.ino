#include <WiFi.h>
#include <ArduinoOTA.h>
#include "connectWifi.h"

const char* ssid = SSID;
const char* password = PW;

void setup() {
  Serial.begin(9600);
  Serial.println("Booting");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {Serial.println("Connect Failed"); delay(5000); ESP.restart();}

  // Cấu hình OTA
  ArduinoOTA.setHostname("ESP32_OTA"); // tên để tìm trong Arduino IDE
  ArduinoOTA.setPassword("esp32"); // (tùy chọn) thêm password để bảo mật

  ArduinoOTA
    .onStart([]() {Serial.println("Start OTA update...");})
    .onEnd([]() {Serial.println("\nCompleted OTA!");})
    .onProgress([](unsigned int progress, unsigned int total) {Serial.printf("Progress: %u%%\r", (progress / (total / 100)));})
    .onError([](ota_error_t error) {Serial.printf("Error [%u]: ", error); if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed"); else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed"); else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed"); else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed"); else if (error == OTA_END_ERROR) Serial.println("End Failed");});
  ArduinoOTA.begin();
  Serial.println("Ready IP: ");Serial.println(WiFi.localIP());
}

void loop() 
{
  ArduinoOTA.handle(); // xử lý OTA
}
