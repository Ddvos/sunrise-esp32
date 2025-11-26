#include "mqtt_manager.h"
#include "wifi_manager.h"
#include "config.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

static WiFiClientSecure wifiClient;
static PubSubClient mqttClient(wifiClient);

static MqttState currentState = MQTT_STATE_DISCONNECTED;
static MqttMessageCallback userCallback = nullptr;
static unsigned long lastRetryTime = 0;

// Internal callback that wraps PubSubClient callback
static void internalCallback(char* topic, byte* payload, unsigned int length) {
  if (userCallback == nullptr) return;

  // Convert payload to null-terminated string
  static char msgBuffer[256];
  unsigned int copyLen = (length < sizeof(msgBuffer) - 1) ? length : sizeof(msgBuffer) - 1;
  memcpy(msgBuffer, payload, copyLen);
  msgBuffer[copyLen] = '\0';

  // Trim whitespace
  char* msg = msgBuffer;
  while (*msg == ' ' || *msg == '\n' || *msg == '\r') msg++;
  char* end = msg + strlen(msg) - 1;
  while (end > msg && (*end == ' ' || *end == '\n' || *end == '\r')) {
    *end = '\0';
    end--;
  }

  userCallback(topic, msg);
}

void mqtt_init() {
  wifiClient.setCACert(MQTT_CA_CERT);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(internalCallback);

  currentState = MQTT_STATE_DISCONNECTED;
  lastRetryTime = 0;
}

void mqtt_update() {
  // Don't try MQTT if WiFi isn't connected
  if (!wifi_isConnected()) {
    if (currentState == MQTT_STATE_CONNECTED) {
      currentState = MQTT_STATE_DISCONNECTED;
    }
    return;
  }

  switch (currentState) {
    case MQTT_STATE_DISCONNECTED:
      // Start connection attempt
      currentState = MQTT_STATE_CONNECTING;
      lastRetryTime = millis();
      Serial.print("Connecting to MQTT...");

      if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
        currentState = MQTT_STATE_CONNECTED;
        Serial.println("connected!");

        // Subscribe to LED topic
        mqttClient.subscribe(LED_TOPIC);
        Serial.print("Subscribed to ");
        Serial.println(LED_TOPIC);
      } else {
        currentState = MQTT_STATE_DISCONNECTED;
        Serial.print("failed, state=");
        Serial.println(mqttClient.state());
        lastRetryTime = millis();
      }
      break;

    case MQTT_STATE_CONNECTING:
      // Check if connection succeeded
      if (mqttClient.connected()) {
        currentState = MQTT_STATE_CONNECTED;
      } else if (millis() - lastRetryTime >= MQTT_RETRY_INTERVAL) {
        // Retry
        currentState = MQTT_STATE_DISCONNECTED;
      }
      break;

    case MQTT_STATE_CONNECTED:
      // Keep connection alive
      if (!mqttClient.connected()) {
        currentState = MQTT_STATE_DISCONNECTED;
        Serial.println("MQTT connection lost!");
      } else {
        mqttClient.loop();
      }
      break;
  }
}

MqttState mqtt_getState() {
  return currentState;
}

bool mqtt_isConnected() {
  return currentState == MQTT_STATE_CONNECTED && mqttClient.connected();
}

void mqtt_setCallback(MqttMessageCallback callback) {
  userCallback = callback;
}

bool mqtt_publish(const char* topic, const char* payload) {
  if (!mqtt_isConnected()) return false;
  return mqttClient.publish(topic, payload);
}
