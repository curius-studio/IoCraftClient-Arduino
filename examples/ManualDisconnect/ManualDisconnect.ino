/*
  IoCraft - ManualDisconnect Example

  Demonstrates how to manually close the connection
  and prevent automatic reconnection.

  Requirements:
  - Device registered via /ioc menu
  - SECRET generated in Minecraft
*/

#include <ESP8266WiFi.h>
#include <IoCraftClient.h>

IoCraftClient iocraft;

unsigned long startTime = 0;
bool disconnected = false;

void setup() {
  Serial.begin(115200);

  // WiFi connection
  WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");

  // Disable auto-reconnect
  iocraft.setAutoReconnect(false);

  // Device identifier
  String deviceName = "ESP-" + String(ESP.getChipId());

  Serial.print("Device ID: ");
  Serial.println(deviceName);

  // Connect to IoCraft server
  iocraft.begin("192.168.1.100", 8765, deviceName);

  // Enable Serial-based authentication
  iocraft.enableSerialAuth();

  Serial.println("Paste the SECRET in the Serial Monitor and press ENTER");

  startTime = millis();
}

void loop() {
  iocraft.loop();

  // Disconnect after 10 seconds
  if (!disconnected && millis() - startTime > 10000) {

    Serial.println("\nDisconnecting manually...");
    iocraft.disconnect();

    disconnected = true;
  }

  // Check connection status after disconnect
  if (disconnected) {

    static unsigned long lastCheck = 0;

    if (millis() - lastCheck > 3000) {
      lastCheck = millis();

      if (iocraft.isConnected()) {
        Serial.println("Still connected");
      } else {
        Serial.println("Connection closed");
      }
    }
  }
}