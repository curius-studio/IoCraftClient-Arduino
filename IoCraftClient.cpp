#include "IoCraftClient.h"
#include <bearssl/bearssl_hmac.h>
#include <time.h>

extern "C" {
  #include "user_interface.h"
}

IoCraftClient* IoCraftClient::instance = nullptr;

/* -------------------------------------------------------------------------- */
/*                              Utility Functions                              */
/* -------------------------------------------------------------------------- */

static String hmacSha256(const String& key, const String& data) {
  br_hmac_key_context kc;
  br_hmac_context hc;
  uint8_t out[32];

  br_hmac_key_init(&kc, &br_sha256_vtable,
                   (const uint8_t*)key.c_str(), key.length());
  br_hmac_init(&hc, &kc, 0);
  br_hmac_update(&hc, (const uint8_t*)data.c_str(), data.length());
  br_hmac_out(&hc, out);

  String hex;
  for (int i = 0; i < 32; i++) {
    if (out[i] < 0x10) hex += "0";
    hex += String(out[i], HEX);
  }
  return hex;
}

/* -------------------------------------------------------------------------- */
/*                               Initialization                               */
/* -------------------------------------------------------------------------- */

void IoCraftClient::begin(const char* host, uint16_t port, String deviceName) {
  instance = this;
  _deviceName = deviceName;
  _manualDisconnect = false;

  String path = "/?name=" + deviceName;

  ws.begin(host, port, path.c_str());
  ws.onEvent(_webSocketEvent);

  if (_autoReconnect) {
    ws.setReconnectInterval(5000);
  } else {
    ws.setReconnectInterval(0);
  }

  ws.enableHeartbeat(15000, 3000, 2);

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("IoCraft: synchronizing time");

  time_t now = time(nullptr);
  while (now < 100000) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }

  Serial.println("\nIoCraft: time synchronized");
}

/* -------------------------------------------------------------------------- */
/*                                   Loop                                     */
/* -------------------------------------------------------------------------- */

void IoCraftClient::loop() {
  if (_manualDisconnect) return;

  ws.loop();

  if (_serialAuthEnabled && Serial.available()) {
    String secret = Serial.readStringUntil('\n');
    secret.trim();

    if (secret.length() == 64) {
      Serial.println("IoCraft: secret received via Serial");
      authenticate(secret);
    } else {
      Serial.println("IoCraft: invalid secret (64 chars required)");
    }
  }
}

/* -------------------------------------------------------------------------- */
/*                               Configuration                                */
/* -------------------------------------------------------------------------- */

void IoCraftClient::setAutoReconnect(bool enabled) {
  _autoReconnect = enabled;
}

void IoCraftClient::enableSerialAuth() {
  _serialAuthEnabled = true;
  Serial.println("IoCraft: serial authentication enabled");
}

/* -------------------------------------------------------------------------- */
/*                               Connection State                             */
/* -------------------------------------------------------------------------- */

bool IoCraftClient::isConnected() {
  return _connected;
}

bool IoCraftClient::isAuthenticated() {
  return _authenticated;
}

/* -------------------------------------------------------------------------- */
/*                              Message Handling                              */
/* -------------------------------------------------------------------------- */

void IoCraftClient::onMessage(std::function<void(String)> callback) {
  _onMessage = callback;
}

/* -------------------------------------------------------------------------- */
/*                                Authentication                              */
/* -------------------------------------------------------------------------- */

void IoCraftClient::authenticate(String secret) {
  if (!_connected) {
    Serial.println("IoCraft: not connected");
    return;
  }

  _secret = secret;

  Serial.println("IoCraft: authenticating");

  time_t now = time(nullptr);
  unsigned long long ts = (unsigned long long)now * 1000ULL;

  String nonce = String(os_random()) + "-" + String(os_random());
  String message = _deviceName + ":" + String(ts) + ":" + nonce;
  String sig = hmacSha256(secret, message);

  String payload = "{";
  payload += "\"type\":\"hello\",";
  payload += "\"device\":\"" + _deviceName + "\",";
  payload += "\"ts\":" + String(ts) + ",";
  payload += "\"nonce\":\"" + nonce + "\",";
  payload += "\"sig\":\"" + sig + "\"";
  payload += "}";

  ws.sendTXT(payload);

  Serial.println("[TX] " + payload);
}

/* -------------------------------------------------------------------------- */
/*                                Disconnection                               */
/* -------------------------------------------------------------------------- */

void IoCraftClient::disconnect() {
  _manualDisconnect = true;
  _authenticated = false;

  ws.enableHeartbeat(0, 0, 0);
  ws.disconnect();

  Serial.println("IoCraft: connection closed manually");
}

/* -------------------------------------------------------------------------- */
/*                                 Send Methods                               */
/* -------------------------------------------------------------------------- */

void IoCraftClient::send(String message) {
  if (!_connected) {
    Serial.println("IoCraft: not connected");
    return;
  }

  if (!_authenticated) {
    Serial.println("IoCraft: not authenticated");
    return;
  }

  ws.sendTXT(message);
}

void IoCraftClient::sendCmd(String command) {
  if (!_connected) {
    Serial.println("IoCraft: not connected");
    return;
  }

  if (!_authenticated) {
    Serial.println("IoCraft: not authenticated (cmd blocked)");
    return;
  }

  String payload = "{";
  payload += "\"type\":\"cmd\",";
  payload += "\"data\":\"" + command + "\"";
  payload += "}";

  ws.sendTXT(payload);

  Serial.println("[CMD] " + payload);
}

void IoCraftClient::sendToBlock(int x, int y, int z, String data) {
  if (!_connected) {
    Serial.println("IoCraft: not connected");
    return;
  }

  if (!_authenticated) {
    Serial.println("IoCraft: not authenticated");
    return;
  }

  String payload = "{";
  payload += "\"type\":\"sensor\",";
  payload += "\"x\":" + String(x) + ",";
  payload += "\"y\":" + String(y) + ",";
  payload += "\"z\":" + String(z) + ",";
  payload += "\"data\":\"" + data + "\",";
  payload += "\"device\":\"" + _deviceName + "\"";
  payload += "}";

  ws.sendTXT(payload);

  Serial.println("[BLOCK] " + payload);
}

/* -------------------------------------------------------------------------- */
/*                              WebSocket Events                              */
/* -------------------------------------------------------------------------- */

void IoCraftClient::_webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if (!instance) return;

  switch (type) {

    case WStype_CONNECTED:
      instance->_connected = true;
      Serial.println("IoCraft: connected");
      break;

    case WStype_DISCONNECTED:
      instance->_connected = false;
      Serial.println("IoCraft: disconnected");

      if (instance->_manualDisconnect) {
        Serial.println("IoCraft: reconnect blocked (manual)");
        return;
      }

      if (!instance->_autoReconnect) {
        Serial.println("IoCraft: auto-reconnect disabled");
        return;
      }

      break;

    case WStype_TEXT: {
      String msg;
      for (size_t i = 0; i < length; i++) {
        msg += (char)payload[i];
      }

      if (msg.indexOf("\"type\":\"hello/ack\"") != -1 &&
          msg.indexOf("\"ok\":true") != -1) {
        instance->_authenticated = true;
        Serial.println("IoCraft: authenticated");
      }

      if (instance->_onMessage) {
        instance->_onMessage(msg);
      }

      break;
    }

    case WStype_ERROR:
      Serial.println("IoCraft: error");
      break;

    default:
      break;
  }
}