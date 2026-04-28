/*
  IoCraft - SendCommand Example

  Envía comandos al servidor Minecraft usando IoCraft.
  Requiere rol: cmd
*/

#include <ESP8266WiFi.h>
#include <IoCraftClient.h>

IoCraftClient iocraft;

bool enviado = false;

void setup() {
  Serial.begin(115200);

  WiFi.begin("TU_SSID", "TU_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  String deviceName = "ESP-" + String(ESP.getChipId());

  iocraft.setAutoReconnect(false);
  iocraft.begin("192.168.1.100", 8765, deviceName);
  iocraft.enableSerialAuth();

  Serial.println("📌 Pega el SECRET en el monitor serial");
}

void loop() {
  iocraft.loop();

  if (!enviado && iocraft.isAuthenticated()) {

    Serial.println("🚀 Enviando comando...");

    iocraft.sendCmd("say Hola desde IoCraft 🚀");

    enviado = true;
  }
}