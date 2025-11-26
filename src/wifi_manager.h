#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

// WiFi connection states
enum WifiState {
  WIFI_STATE_DISCONNECTED,
  WIFI_STATE_CONNECTING,
  WIFI_STATE_CONNECTED,
  WIFI_STATE_FAILED
};

// Initialize WiFi manager
void wifi_init();

// Update WiFi manager (call in loop) - non-blocking
void wifi_update();

// Get current WiFi state
WifiState wifi_getState();

// Check if WiFi is connected
bool wifi_isConnected();

// Get IP address as string
String wifi_getIP();

#endif // WIFI_MANAGER_H
