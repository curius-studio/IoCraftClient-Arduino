```cpp
/*
  IoCraft - BasicConnection Example

  Demonstrates a basic connection to the IoCraft server,
  authentication via Serial, and sending initial messages.

  Requirements:
  - Device registered via /ioc menu
  - SECRET generated in Minecraft
  - Correct roles assigned (cmd, sensor)
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
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // IoCraft client configuration
  iocraft.setAutoReconnect(false);

  String deviceName = "ESP-" + String(ESP.getChipId());

  Serial.print("Device ID: ");
  Serial.println(deviceName);

  // Connect to IoCraft server
  iocraft.begin("192.168.1.100", 8765, deviceName);

  // Enable Serial-based authentication
  iocraft.enableSerialAuth();

  // Incoming message handler
  iocraft.onMessage([](String msg) {
    Serial.print("RX: ");
    Serial.println(msg);
  });

  Serial.println("\nPaste the SECRET in the Serial Monitor and press ENTER");
}

void loop() {
  iocraft.loop();

  // Execute once after authentication
  if (!sent && iocraft.isAuthenticated()) {

    Serial.println("\nAuthenticated");
    Serial.println("Sending test messages...\n");

    // Send command to Minecraft
    iocraft.sendCmd("say Hello from IoCraft");

    // Send data to block (adjust coordinates as needed)
    iocraft.sendToBlock(1, 64, 10, "Temp:25C");

    sent = true;
  }
}
```
