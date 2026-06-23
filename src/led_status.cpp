#include "led_status.h"

static unsigned long ledRxUntil = 0;
static unsigned long ledSaveUntil = 0;

void ledInit() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void blinkPattern(uint8_t count, uint16_t onMs, uint16_t offMs) {
  for (uint8_t i = 0; i < count; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(onMs);
    digitalWrite(LED_BUILTIN, LOW);
    delay(offMs);
  }
}

void pulseRxLed() {
  ledRxUntil = millis() + 250;
}

void pulseSaveLed() {
  ledSaveUntil = millis() + 180;
}

void updateLed() {
  const unsigned long now = millis();
  if (now < ledSaveUntil) {
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }
  if (now < ledRxUntil) {
    digitalWrite(LED_BUILTIN, (now / 60) & 1);
    return;
  }
  digitalWrite(LED_BUILTIN, LOW);
}
