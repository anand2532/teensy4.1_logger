#pragma once

#include <Arduino.h>

void blinkPattern(uint8_t count, uint16_t onMs = 120, uint16_t offMs = 120);
void pulseRxLed();
void pulseSaveLed();
void updateLed();
void ledInit();
