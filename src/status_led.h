#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <Arduino.h>

// Status LED colors/states
enum StatusColor {
  STATUS_OFF,
  STATUS_BOOTING,       // Green - system starting
  STATUS_WIFI_CONNECTING,  // Yellow - connecting to WiFi
  STATUS_WIFI_FAILED,   // Red - WiFi connection failed
  STATUS_MQTT_CONNECTING,  // Cyan - connecting to MQTT
  STATUS_CONNECTED,     // Blue - fully connected
  STATUS_ERROR          // Red blinking - error state
};

// Initialize status LED
void statusLed_init();

// Update status LED (call in loop) - handles blinking
void statusLed_update();

// Set status LED color/state
void statusLed_setStatus(StatusColor status);

// Get current status
StatusColor statusLed_getStatus();

// Manual RGB control (0-255 per channel)
void statusLed_setRGB(uint8_t r, uint8_t g, uint8_t b);

// Turn off status LED
void statusLed_off();

#endif // STATUS_LED_H
