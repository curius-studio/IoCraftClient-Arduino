#ifndef IOCRAFT_CLIENT_H
#define IOCRAFT_CLIENT_H

#include <WebSocketsClient.h>
#include <functional>

class IoCraftClient {
public:
  /* -------------------------------------------------------------------------- */
  /*                               Core Lifecycle                               */
  /* -------------------------------------------------------------------------- */

  void begin(const char* host, uint16_t port, String deviceName);
  void loop();

  /* -------------------------------------------------------------------------- */
  /*                              Connection Control                            */
  /* -------------------------------------------------------------------------- */

  void disconnect();
  void setAutoReconnect(bool enabled);

  /* -------------------------------------------------------------------------- */
  /*                               Authentication                              */
  /* -------------------------------------------------------------------------- */

  void authenticate(String secret);
  void enableSerialAuth();

  /* -------------------------------------------------------------------------- */
  /*                                State Access                                */
  /* -------------------------------------------------------------------------- */

  bool isConnected();
  bool isAuthenticated();

  /* -------------------------------------------------------------------------- */
  /*                              Message Handling                              */
  /* -------------------------------------------------------------------------- */

  void onMessage(std::function<void(String)> callback);

  /* -------------------------------------------------------------------------- */
  /*                                 Messaging                                  */
  /* -------------------------------------------------------------------------- */

  void send(String message);
  void sendCmd(String command);
  void sendToBlock(int x, int y, int z, String data);

private:
  /* -------------------------------------------------------------------------- */
  /*                               Internal State                               */
  /* -------------------------------------------------------------------------- */

  WebSocketsClient ws;
  String _deviceName;

  bool _manualDisconnect = false;
  bool _autoReconnect = true;
  bool _connected = false;
  bool _authenticated = false;
  bool _serialAuthEnabled = false;

  String _secret = "";

  std::function<void(String)> _onMessage;

  /* -------------------------------------------------------------------------- */
  /*                           Internal Static Handler                          */
  /* -------------------------------------------------------------------------- */

  static IoCraftClient* instance;
  static void _webSocketEvent(WStype_t type, uint8_t* payload, size_t length);
};

#endif