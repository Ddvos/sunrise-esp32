#include "led_controller.h"
#include "config.h"

static LedMode currentMode = LED_MODE_OFF;

// Test cycle state
static int brightness = 0;
static bool fadingUp = true;
static int currentColor = 0;  // 0=WW, 1=Red, 2=Green, 3=Blue
static const int NUM_COLORS = 4;
static unsigned long lastFadeTime = 0;

// Current brightness values
static uint16_t wwBrightness = 0;
static uint16_t redBrightness = 0;
static uint16_t greenBrightness = 0;
static uint16_t blueBrightness = 0;

static const char* colorNames[] = {"Warm White", "Red", "Green", "Blue"};

static int getCurrentChannel() {
  switch (currentColor) {
    case 0: return WW_CHANNEL;
    case 1: return RED_CHANNEL;
    case 2: return GREEN_CHANNEL;
    case 3: return BLUE_CHANNEL;
    default: return WW_CHANNEL;
  }
}

void ledController_init() {
  // Configure PWM for all LED channels
  ledcSetup(WW_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(RED_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(GREEN_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(BLUE_CHANNEL, PWM_FREQ, PWM_RESOLUTION);

  // Attach pins to channels
  ledcAttachPin(WW_LED_PIN, WW_CHANNEL);
  ledcAttachPin(RED_LED_PIN, RED_CHANNEL);
  ledcAttachPin(GREEN_LED_PIN, GREEN_CHANNEL);
  ledcAttachPin(BLUE_LED_PIN, BLUE_CHANNEL);

  // Start with all LEDs off
  ledController_off();

  currentMode = LED_MODE_OFF;
  lastFadeTime = millis();
}

void ledController_update() {
  unsigned long currentTime = millis();

  switch (currentMode) {
    case LED_MODE_OFF:
      // Nothing to do
      break;

    case LED_MODE_TEST_CYCLE:
      // Non-blocking fade using millis()
      if (currentTime - lastFadeTime >= LED_FADE_INTERVAL) {
        lastFadeTime = currentTime;

        // Write brightness to current color channel
        ledcWrite(getCurrentChannel(), brightness);

        // Update brightness
        if (fadingUp) {
          brightness += LED_FADE_STEP;
          if (brightness >= PWM_MAX_VALUE) {
            brightness = PWM_MAX_VALUE;
            fadingUp = false;
          }
        } else {
          brightness -= LED_FADE_STEP;
          if (brightness <= 0) {
            brightness = 0;
            fadingUp = true;

            // Move to next color
            currentColor = (currentColor + 1) % NUM_COLORS;
            Serial.print("Switching to: ");
            Serial.println(colorNames[currentColor]);
          }
        }
      }
      break;

    case LED_MODE_SUNRISE:
      // TODO: Implement sunrise simulation
      break;

    case LED_MODE_MANUAL:
      // Manual mode - brightness controlled via setters
      break;
  }
}

void ledController_setMode(LedMode mode) {
  if (mode == currentMode) return;

  // Reset state when changing modes
  currentMode = mode;

  switch (mode) {
    case LED_MODE_OFF:
      ledController_off();
      break;

    case LED_MODE_TEST_CYCLE:
      brightness = 0;
      fadingUp = true;
      currentColor = 0;
      lastFadeTime = millis();
      ledController_off();  // Start with all off
      Serial.println("LED test cycle started");
      Serial.print("Starting with: ");
      Serial.println(colorNames[currentColor]);
      break;

    case LED_MODE_SUNRISE:
      // TODO: Initialize sunrise state
      break;

    case LED_MODE_MANUAL:
      // Keep current brightness values
      break;
  }
}

LedMode ledController_getMode() {
  return currentMode;
}

void ledController_setWW(uint16_t value) {
  wwBrightness = constrain(value, 0, PWM_MAX_VALUE);
  ledcWrite(WW_CHANNEL, wwBrightness);
}

void ledController_setRed(uint16_t value) {
  redBrightness = constrain(value, 0, PWM_MAX_VALUE);
  ledcWrite(RED_CHANNEL, redBrightness);
}

void ledController_setGreen(uint16_t value) {
  greenBrightness = constrain(value, 0, PWM_MAX_VALUE);
  ledcWrite(GREEN_CHANNEL, greenBrightness);
}

void ledController_setBlue(uint16_t value) {
  blueBrightness = constrain(value, 0, PWM_MAX_VALUE);
  ledcWrite(BLUE_CHANNEL, blueBrightness);
}

void ledController_setAll(uint16_t ww, uint16_t r, uint16_t g, uint16_t b) {
  ledController_setWW(ww);
  ledController_setRed(r);
  ledController_setGreen(g);
  ledController_setBlue(b);
}

void ledController_off() {
  wwBrightness = 0;
  redBrightness = 0;
  greenBrightness = 0;
  blueBrightness = 0;

  ledcWrite(WW_CHANNEL, 0);
  ledcWrite(RED_CHANNEL, 0);
  ledcWrite(GREEN_CHANNEL, 0);
  ledcWrite(BLUE_CHANNEL, 0);
}

const char* ledController_getCurrentColorName() {
  return colorNames[currentColor];
}
