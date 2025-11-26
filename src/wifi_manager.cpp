#include "wifi_manager.h"
#include "config.h"
#include <WiFi.h>

static WifiState currentState = WIFI_STATE_DISCONNECTED;
static unsigned long lastRetryTime = 0;
static int retryCount = 0;

void wifi_init() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  currentState = WIFI_STATE_CONNECTING;
  lastRetryTime = millis();
  retryCount = 0;
}

void wifi_update() {
  switch (currentState) {
    case WIFI_STATE_CONNECTING:
      if (WiFi.status() == WL_CONNECTED) {
        currentState = WIFI_STATE_CONNECTED;
        Serial.println();
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
      } else if (millis() - lastRetryTime >= WIFI_RETRY_INTERVAL) {
        lastRetryTime = millis();
        retryCount++;
        Serial.print(".");

        if (retryCount >= WIFI_MAX_RETRIES) {
          currentState = WIFI_STATE_FAILED;
          Serial.println();
          Serial.println("Failed to connect to WiFi");
        }
      }
      break;

    case WIFI_STATE_CONNECTED:
      // Check if we lost connection
      if (WiFi.status() != WL_CONNECTED) {
        currentState = WIFI_STATE_DISCONNECTED;
        Serial.println("WiFi connection lost!");
      }
      break;

    case WIFI_STATE_DISCONNECTED:
      // Auto-reconnect
      Serial.println("Reconnecting to WiFi...");
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      currentState = WIFI_STATE_CONNECTING;
      lastRetryTime = millis();
      retryCount = 0;
      break;

    case WIFI_STATE_FAILED:
      // Could implement retry after longer delay here
      break;
  }
}

WifiState wifi_getState() {
  return currentState;
}

bool wifi_isConnected() {
  return currentState == WIFI_STATE_CONNECTED;
}

String wifi_getIP() {
  if (wifi_isConnected()) {
    return WiFi.localIP().toString();
  }
  return "0.0.0.0";
}
