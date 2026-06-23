#include "sd_logger.h"

#include <SD.h>

#include "config.h"
#include "led_status.h"

static File dataLog;
static File diagLog;
static uint32_t linesLogged = 0;
static uint32_t writeErrors = 0;

static bool initSdCard() {
  for (uint8_t attempt = 0; attempt < SD_RETRY_COUNT; attempt++) {
    if (SD.begin(BUILTIN_SDCARD)) {
      if (!SD.exists(LOG_DIR) && !SD.mkdir(LOG_DIR)) {
        delay(200);
        continue;
      }
      return true;
    }
    delay(300);
  }
  return false;
}

static bool verifySdWrite() {
  File test = SD.open("logs/_sdtest.txt", FILE_WRITE);
  if (!test) {
    return false;
  }
  const char probe[] = "sd_ok\n";
  if (test.write(probe, sizeof(probe) - 1) != sizeof(probe) - 1) {
    test.close();
    return false;
  }
  test.flush();
  test.close();
  return SD.exists("logs/_sdtest.txt");
}

bool sdLoggerInit() {
  return initSdCard() && verifySdWrite();
}

bool sdLoggerOpenFiles() {
  dataLog = SD.open(DATA_LOG, FILE_WRITE);
  diagLog = SD.open(DIAG_LOG, FILE_WRITE);
  return static_cast<bool>(dataLog) && static_cast<bool>(diagLog);
}

void sdDiagMsg(const char *msg) {
  if (!diagLog) {
    return;
  }
  diagLog.print('[');
  diagLog.print(millis());
  diagLog.print("] ");
  diagLog.println(msg);
  diagLog.flush();
}

bool sdWriteLogLine(const char *text) {
  if (!dataLog) {
    return false;
  }

  dataLog.print('[');
  dataLog.print(millis());
  dataLog.print("] ");
  dataLog.println(text);
  dataLog.flush();

  if (dataLog.getWriteError()) {
    writeErrors++;
    dataLog.clearWriteError();
    sdDiagMsg("ERROR data.log write failed");
    return false;
  }

  linesLogged++;
  pulseSaveLed();
  return true;
}

uint32_t sdLinesLogged() {
  return linesLogged;
}

uint32_t sdWriteErrors() {
  return writeErrors;
}
