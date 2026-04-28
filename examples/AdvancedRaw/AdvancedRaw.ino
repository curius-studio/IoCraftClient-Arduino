/*
  IoCraft - AdvancedRaw Example

  Sends a custom JSON message directly to the IoCraft server.

  Advanced usage:
  Requires understanding of the IoCraft protocol and correct JSON structure.

  Requirements:
  - Device registered via /ioc menu
  - SECRET generated in Minecraft
*/

#include <ESP8266WiFi.h>
#include <IoCraftClient.h>

IoCraftClient iocraft;

bool sent = false;

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

    Serial.println("\nSending RAW message...\n");

    // Manual JSON payload (example: Minecraft command)
    String payload = "{";
    payload += "\"type\":\"cmd\",";
    payload += "\"data\":\"time set day\"";
    payload += "}";

    iocraft.sendRaw(payload);

    sent = true;
  }
}