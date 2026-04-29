/*
  IoCraft - AutoSessionRenew Example

  Demonstrates automatic session renewal without user intervention.

  CONCEPTS
  ----------------------------------------------------------------
  - The device SECRET is permanent (until rotated by admin).
  - The SESSION expires every 10 minutes (server-side).
  - When expired, the server rejects messages with "unauthenticated".
  - Solution: re-authenticate using the same SECRET before expiration.

  FLOW
  ----------------------------------------------------------------
  1. Connect to WiFi
  2. Receive SECRET via Serial
  3. Authenticate (hello)
  4. Renew session before expiration
  5. Continue operation indefinitely

  REQUIREMENTS
  ----------------------------------------------------------------
  - Device registered via /ioc menu
  - Valid SECRET (64 hex characters)
  - Role: sensor or cmd
*/

#include <ESP8266WiFi.h>
#include <IoCraftClient.h>

/* -------------------------------------------------------------------------- */
/*                                Configuration                               */
/* -------------------------------------------------------------------------- */

const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASSWORD";

const char* MC_HOST = "192.168.1.5";
const int   MC_PORT = 8765;

// Session TTL (defined by IoCraft server)
const unsigned long SESSION_TTL_MS = 10UL * 60UL * 1000UL;

// Renew session 60 seconds before expiration
const unsigned long SESSION_RENEW_BEFORE_MS = 60UL * 1000UL;

/* -------------------------------------------------------------------------- */
/*                                   State                                    */
/* -------------------------------------------------------------------------- */

IoCraftClient iocraft;

String secret = "";
bool secretReceived = false;

unsigned long sessionStartMs = 0;

/* -------------------------------------------------------------------------- */
/*                               Helper Methods                               */
/* -------------------------------------------------------------------------- */

unsigned long getSessionRemaining() {
  if (sessionStartMs == 0) return 0;

  unsigned long elapsed = millis() - sessionStartMs;
  if (elapsed >= SESSION_TTL_MS) return 0;

  return SESSION_TTL_MS - elapsed;
}

bool shouldRenewSession() {
  if (!iocraft.isAuthenticated()) return false;
  return getSessionRemaining() <= SESSION_RENEW_BEFORE_MS;
}

void renewSession() {
  Serial.println("[session] renewing session");

  iocraft.authenticate(secret);
  sessionStartMs = millis();
}

/* -------------------------------------------------------------------------- */
/*                                    Setup                                   */
/* -------------------------------------------------------------------------- */

void setup() {
  Serial.begin(115200);
  delay(100);

  /* ------------------------------- WiFi ----------------------------------- */

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  /* ------------------------------ IoCraft --------------------------------- */

  String deviceName = "ESP-" + String(ESP.getChipId());

  Serial.print("Device ID: ");
  Serial.println(deviceName);

  iocraft.setAutoReconnect(true);
  iocraft.begin(MC_HOST, MC_PORT, deviceName);

  iocraft.onMessage([](String msg) {
    Serial.print("[RX] ");
    Serial.println(msg);
  });

  Serial.println("Paste the SECRET (64 chars) in Serial Monitor");
}

/* -------------------------------------------------------------------------- */
/*                                     Loop                                   */
/* -------------------------------------------------------------------------- */

void loop() {
  iocraft.loop();

  /* ----------------------- 1. Receive SECRET ------------------------------- */

  if (!secretReceived && Serial.available()) {

    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() == 64) {

      secret = input;
      secretReceived = true;

      Serial.println("Secret received → authenticating");

      iocraft.authenticate(secret);
      sessionStartMs = millis();

    } else {
      Serial.println("Invalid SECRET (must be 64 characters)");
    }
  }

  /* --------------------- 2. Session Renewal ------------------------------- */

  if (secretReceived && shouldRenewSession()) {
    renewSession();
  }

  /* ----------------------- 3. Example Logic ------------------------------- */

  static unsigned long lastSend = 0;

  if (iocraft.isAuthenticated() && millis() - lastSend > 30000) {

    lastSend = millis();

    float temp = 20.0 + (millis() % 1000) / 100.0;
    String data = "Temp:" + String(temp, 1) + "C";

    iocraft.sendToBlock(1, 64, 10, data);

    Serial.print("[TX] ");
    Serial.println(data);
  }
}
