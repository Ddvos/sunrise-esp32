#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

// LED modes
enum LedMode {
  LED_MODE_OFF,
  LED_MODE_TEST_CYCLE,    // Cycle through WW, R, G, B for testing
  LED_MODE_SUNRISE,       // Sunrise simulation (to be implemented)
  LED_MODE_MANUAL         // Manual control via MQTT
};

// Initialize LED controller
void ledController_init();

// Update LED controller (call in loop) - non-blocking
void ledController_update();

// Set LED mode
void ledController_setMode(LedMode mode);

// Get current LED mode
LedMode ledController_getMode();

// Manual control functions
void ledController_setWW(uint16_t brightness);      // 0-4095
void ledController_setRed(uint16_t brightness);     // 0-4095
void ledController_setGreen(uint16_t brightness);   // 0-4095
void ledController_setBlue(uint16_t brightness);    // 0-4095
void ledController_setAll(uint16_t ww, uint16_t r, uint16_t g, uint16_t b);

// Turn all LEDs off
void ledController_off();

// Get current color name (for test cycle mode)
const char* ledController_getCurrentColorName();

#endif // LED_CONTROLLER_H
