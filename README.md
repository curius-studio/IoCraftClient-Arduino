# IoCraftClient

![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue?logo=arduino)
![ESP8266](https://img.shields.io/badge/ESP8266-Supported-green)
![ESP32](https://img.shields.io/badge/ESP32-Experimental-yellow)
![License](https://img.shields.io/badge/License-MIT-purple)
![Status](https://img.shields.io/badge/Status-Active-brightgreen)

Arduino library for connecting devices (ESP8266 / ESP32) to **IoCraft** via WebSocket.

This library simplifies the connection and authentication process, allowing you to interact with Minecraft using a clean and minimal API.

---

## 🚀 Features

* 🔌 Easy WebSocket connection to IoCraft
* 🔐 Built-in authentication (HMAC + nonce)
* 🔄 Optional auto-reconnection
* 🔴 Manual disconnect support (no ghost reconnections)
* 📡 Receive messages with callbacks
* 📊 Connection state tracking (`isConnected()`, `isAuthenticated()`)
* 🎮 Send commands to Minecraft (`sendCmd`)
* 🧱 Send data to blocks (`sendToBlock`)
* ⚙️ Advanced raw messaging (`sendRaw`)

---

## 🧠 Philosophy

IoCraftClient is a **helper library**, not a requirement.

* It does NOT replace the official IoCraft protocol
* It does NOT restrict how you interact with the server
* It simply reduces boilerplate and common errors

You are free to implement your own client using WebSocket as long as you follow IoCraft's protocol.

For advanced use cases, `sendRaw()` allows full control.

---

## 📦 Installation

### Option 1 — Manual

1. Download or clone this repository
2. Copy the folder into:

```
Documents/Arduino/libraries/
```

3. Restart Arduino IDE

---

### Option 2 — ZIP

1. Download the repository as `.zip`
2. In Arduino IDE:

```
Sketch → Include Library → Add .ZIP Library
```

---

## ⚡ Basic Example

```cpp
#include <ESP8266WiFi.h>
#include <IoCraftClient.h>

IoCraftClient iocraft;

void setup() {
  Serial.begin(115200);

  WiFi.begin("YOUR_WIFI", "YOUR_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  String name = "ESP-" + String(ESP.getChipId());

  iocraft.begin("192.168.1.5", 8765, name);

  iocraft.enableSerialAuth();

  iocraft.onMessage([](String msg) {
    Serial.println("📩 " + msg);
  });

  Serial.println("Paste your SECRET in Serial Monitor");
}

void loop() {
  iocraft.loop();
}
```

---

## 🔧 API Reference

### Connection

```cpp
iocraft.begin(host, port, deviceName);
iocraft.loop();
iocraft.disconnect();
iocraft.setAutoReconnect(bool enabled);
```

---

### State

```cpp
iocraft.isConnected();
iocraft.isAuthenticated();
```

---

### Authentication

```cpp
iocraft.enableSerialAuth();
iocraft.authenticate(secret);
```

---

### Messaging

#### 🟢 Send command (requires role: cmd)

```cpp
iocraft.sendCmd("say Hello from IoCraft");
```

---

#### 🟢 Send to block (requires role: sensor)

```cpp
iocraft.sendToBlock(x, y, z, "Temp:25°C");
```

---

#### ⚙️ Advanced (raw JSON)

```cpp
iocraft.sendRaw("{\"type\":\"cmd\",\"data\":\"time set day\"}");
```

---

## 📁 Examples

Ready-to-use examples:

```
File → Examples → IoCraftClient
```

Includes:

* BasicConnection
* SendCommand
* SendToBlock
* AdvancedRaw
* ConnectionStatus
* ManualDisconnect

---

## ⚠️ Important Notes

* ESP8266 only supports **2.4GHz WiFi**
* Make sure IoCraft is running in Minecraft
* Use `/ioc info` to get correct IP and port
* Your device must be registered via `/ioc menu`

### Roles

* `cmd` → allows executing commands
* `sensor` → allows sending data to blocks

---

### ⚠️ About message format

IoCraft expects **JSON messages**.

Although the internal parser may accept plain text, it is **not supported** and may cause errors or disconnections.

Always use structured JSON (`cmd`, `sensor`, etc.).

---

## 🔮 Roadmap

* [ ] Event callbacks (`onConnect`, `onDisconnect`)
* [ ] Improved error handling
* [ ] Better cross-platform support
* [ ] Utility helpers (device naming, config)

---

## 👨‍💻 Author

Curius (José Escorcia)

---

## 📄 License

MIT
