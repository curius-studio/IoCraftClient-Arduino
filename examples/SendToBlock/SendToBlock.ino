/*
  IoCraft - SendToBlock Example

  Sends data to a Computer block in Minecraft.

  Requirements:
  - Device registered via /ioc menu
  - SECRET generated in Minecraft
  - Role: sensor
  - Block coordinates must match the target Computer block
*/

#include <ESP8266WiFi.h>
#include <IoCraftClient.h>

IoCraftClient iocraft;

bool sent = false;

// Adjust block coordinates as needed
const int X = 1;
const int Y = 64;
const int Z = 10;

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

  // IoCraft configuration
  iocraft.setAutoReconnect(false);
  iocraft.begin("192.168.1.100", 8765, deviceName);

  // Enable Serial-based authentication
  iocraft.enableSerialAuth();

  Serial.println("Paste the SECRET in the Serial Monitor and press ENTER");
}

void loop() {
  iocraft.loop();

  // Execute once after authentication
  if (!sent && iocraft.isAuthenticated()) {

    Serial.println("Sending data to block...");

    iocraft.sendToBlock(X, Y, Z, "Temp:25C");

    sent = true;
  }
}