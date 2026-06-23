#pragma once

#include <Arduino.h>

void usbHostInputBegin();
void usbHostInputPoll();

bool usbHostSourceReady();
uint32_t usbHostRxBytes();
