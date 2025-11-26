#include "status_led.h"
#include "config.h"

static StatusColor currentStatus = STATUS_OFF;
static unsigned long lastBlinkTime = 0;
static bool blinkState = false;
static const unsigned long BLINK_INTERVAL = 500;  // ms

// Color definitions (R, G, B) - values 0-50 to not be too bright
static const uint8_t COLORS[][3] = {
  {0, 0, 0},      // STATUS_OFF
  {0, 50, 0},     // STATUS_BOOTING - Green
  {50, 30, 0},    // STATUS_WIFI_CONNECTING - Yellow/Orange
  {50, 0, 0},     // STATUS_WIFI_FAILED - Red
  {0, 50, 50},    // STATUS_MQTT_CONNECTING - Cyan
  {0, 0, 50},     // STATUS_CONNECTED - Blue
  {50, 0, 0}      // STATUS_ERROR - Red (blinks)
};

void statusLed_init() {
  currentStatus = STATUS_BOOTING;
  neopixelWrite(RGB_BUILTIN_PIN, COLORS[STATUS_BOOTING][0],
                COLORS[STATUS_BOOTING][1], COLORS[STATUS_BOOTING][2]);
  lastBlinkTime = millis();
}

void statusLed_update() {
  unsigned long currentTime = millis();

  // Handle blinking for error state
  if (currentStatus == STATUS_ERROR) {
    if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = currentTime;
      blinkState = !blinkState;

      if (blinkState) {
        neopixelWrite(RGB_BUILTIN_PIN, COLORS[STATUS_ERROR][0],
                      COLORS[STATUS_ERROR][1], COLORS[STATUS_ERROR][2]);
      } else {
        neopixelWrite(RGB_BUILTIN_PIN, 0, 0, 0);
      }
    }
  }
}

void statusLed_setStatus(StatusColor status) {
  if (status == currentStatus) return;

  currentStatus = status;
  blinkState = true;
  lastBlinkTime = millis();

  // Set color immediately (unless it's a blinking state)
  if (status != STATUS_ERROR) {
    neopixelWrite(RGB_BUILTIN_PIN, COLORS[status][0],
                  COLORS[status][1], COLORS[status][2]);
  }
}

StatusColor statusLed_getStatus() {
  return currentStatus;
}

void statusLed_setRGB(uint8_t r, uint8_t g, uint8_t b) {
  currentStatus = STATUS_OFF;  // Manual control overrides status
  neopixelWrite(RGB_BUILTIN_PIN, r, g, b);
}

void statusLed_off() {
  currentStatus = STATUS_OFF;
  neopixelWrite(RGB_BUILTIN_PIN, 0, 0, 0);
}
