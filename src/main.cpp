#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "led_controller.h"
#include "status_led.h"

// Track previous states for status LED updates
static WifiState prevWifiState = WIFI_STATE_DISCONNECTED;
static MqttState prevMqttState = MQTT_STATE_DISCONNECTED;

// MQTT message handler
void onMqttMessage(const char* topic, const char* payload) {
  Serial.print("MQTT message on ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  if (strcmp(topic, LED_TOPIC) == 0) {
    // Convert payload to lowercase for comparison
    String msg = String(payload);
    msg.toLowerCase();

    if (msg == "on" || msg == "1" || msg == "true") {
      ledController_setMode(LED_MODE_TEST_CYCLE);
    } else if (msg == "off" || msg == "0" || msg == "false") {
      ledController_setMode(LED_MODE_OFF);
    }
  }
}

// Update status LED based on connection states
void updateStatusLed() {
  WifiState wifiState = wifi_getState();
  MqttState mqttState = mqtt_getState();

  // Only update if state changed
  if (wifiState == prevWifiState && mqttState == prevMqttState) {
    return;
  }

  prevWifiState = wifiState;
  prevMqttState = mqttState;

  // Priority: WiFi issues > MQTT issues > Connected
  if (wifiState == WIFI_STATE_FAILED) {
    statusLed_setStatus(STATUS_WIFI_FAILED);
  } else if (wifiState == WIFI_STATE_CONNECTING) {
    statusLed_setStatus(STATUS_WIFI_CONNECTING);
  } else if (wifiState == WIFI_STATE_CONNECTED) {
    if (mqttState == MQTT_STATE_CONNECTED) {
      statusLed_setStatus(STATUS_CONNECTED);
    } else {
      statusLed_setStatus(STATUS_MQTT_CONNECTING);
    }
  } else {
    statusLed_setStatus(STATUS_WIFI_CONNECTING);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== Sunrise Clock Starting ===");

  // Initialize all modules
  statusLed_init();
  ledController_init();
  wifi_init();
  mqtt_init();

  // Set MQTT callback
  mqtt_setCallback(onMqttMessage);

  // Start LED test cycle
  ledController_setMode(LED_MODE_TEST_CYCLE);
}

void loop() {
  // Update all modules (non-blocking)
  wifi_update();
  mqtt_update();
  ledController_update();
  statusLed_update();

  // Update status LED based on connection states
  updateStatusLed();
}
