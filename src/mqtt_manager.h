#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>

// Callback type for incoming MQTT messages
typedef void (*MqttMessageCallback)(const char* topic, const char* payload);

// MQTT connection states
enum MqttState {
  MQTT_STATE_DISCONNECTED,
  MQTT_STATE_CONNECTING,
  MQTT_STATE_CONNECTED
};

// Initialize MQTT manager
void mqtt_init();

// Update MQTT manager (call in loop) - non-blocking
void mqtt_update();

// Get current MQTT state
MqttState mqtt_getState();

// Check if MQTT is connected
bool mqtt_isConnected();

// Set callback for incoming messages
void mqtt_setCallback(MqttMessageCallback callback);

// Publish a message
bool mqtt_publish(const char* topic, const char* payload);

#endif // MQTT_MANAGER_H
