#include <Arduino.h>

#include "config.h"
#include "diagnostics.h"
#include "led_status.h"
#include "line_parser.h"
#include "sd_logger.h"
#include "usb_host_input.h"

static bool loggerReady = false;

void setup() {
  Serial.begin(SERIAL_BAUD);
  ledInit();
  delay(500);

  if (!sdLoggerInit()) {
    Serial.println("SD card init failed");
    while (true) {
      blinkPattern(1, 80, 80);
    }
  }

  blinkPattern(2);

  if (!sdLoggerOpenFiles()) {
    Serial.println("SD log open failed");
    while (true) {
      blinkPattern(3, 60, 60);
    }
  }

  blinkPattern(3);

  sdDiagMsg("=== LOGGER BOOT ===");
  sdDiagMsg("SD card mounted OK");
  sdWriteLogLine("=== LOGGER STARTED ===");

  usbHostInputBegin();

  loggerReady = true;
  Serial.println("Logger ready");
  Serial.println("  Input: USB host port only");
  Serial.println("  SD: logs/data.log, logs/diag.log");
  diagnosticsPrintWiringHelp();
  sdDiagMsg("Logger ready, waiting for USB host input");
}

void loop() {
  if (!loggerReady) {
    return;
  }

  usbHostInputPoll();
  lineParserFlushStale();
  diagnosticsPrintPeriodicStats();
  updateLed();
}
