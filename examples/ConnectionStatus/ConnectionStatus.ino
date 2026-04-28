/*
  IoCraft - ConnectionStatus Example

  Displays the connection and authentication state of the client.

  Requirements:
  - Device registered via /ioc menu
  - SECRET generated in Minecraft
*/

#include <ESP8266WiFi.h>
#include <IoCraftClient.h>

IoCraftClient iocraft;

unsigned long lastCheck = 0;

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

  // Device identifier
  String deviceName = "ESP-" + String(ESP.getChipId());

  Serial.print("Device ID: ");
  Serial.println(deviceName);

  // Connect to IoCraft server
  iocraft.begin("192.168.1.100", 8765, deviceName);

  // Enable Serial-based authentication
  iocraft.enableSerialAuth();

  Serial.println("Paste the SECRET in the Serial Monitor and press ENTER");
}

void loop() {
  iocraft.loop();

  if (millis() - lastCheck > 3000) {
    lastCheck = millis();

    Serial.print("State: ");

    if (!iocraft.isConnected()) {
      Serial.println("Not connected");
      return;
    }

    if (!iocraft.isAuthenticated()) {
      Serial.println("Connected (not authenticated)");
      return;
    }

    Serial.println("Connected and authenticated");
  }
}