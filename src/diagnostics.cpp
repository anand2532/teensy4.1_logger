#include "diagnostics.h"

#include "config.h"
#include "sd_logger.h"
#include "usb_host_input.h"

static unsigned long lastDiagMs = 0;

void diagnosticsPrintWiringHelp() {
  const char *help[] = {
      "=== USB HOST LOGGER ===",
      "Connect source USB to Teensy USB HOST port (bottom of board)",
      "Do not use the micro USB programming port for the source device",
      "=======================",
  };

  for (const char *line : help) {
    Serial.println(line);
    sdDiagMsg(line);
  }
}

void diagnosticsPrintPeriodicStats() {
  if (millis() - lastDiagMs < DIAG_INTERVAL_MS) {
    return;
  }
  lastDiagMs = millis();

  char buf[96];
  snprintf(buf, sizeof(buf),
           "stats usb=%lu lines=%lu usb_ready=%u sd_err=%lu",
           static_cast<unsigned long>(usbHostRxBytes()),
           static_cast<unsigned long>(sdLinesLogged()),
           usbHostSourceReady() ? 1U : 0U,
           static_cast<unsigned long>(sdWriteErrors()));
  sdDiagMsg(buf);

  if (!usbHostSourceReady() && usbHostRxBytes() == 0) {
    sdDiagMsg("NO INPUT: check USB host wiring and source power");
  }
}
