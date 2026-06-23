#include "line_parser.h"

#include "config.h"
#include "led_status.h"
#include "sd_logger.h"

static char lineBuf[LINE_BUF_SIZE];
static size_t lineLen = 0;
static unsigned long lastCharMs = 0;

static void processLine() {
  if (lineLen == 0) {
    return;
  }
  lineBuf[lineLen] = '\0';
  sdWriteLogLine(lineBuf);
  lineLen = 0;
}

static void appendChar(char c) {
  lastCharMs = millis();
  pulseRxLed();

  if (lineLen + 1 < LINE_BUF_SIZE) {
    lineBuf[lineLen++] = c;
    return;
  }

  processLine();
  lineBuf[lineLen++] = c;
}

void lineParserOnByte(char c) {
  pulseRxLed();

  if (c == '\n') {
    processLine();
  } else if (c != '\r') {
    appendChar(c);
  }
}

void lineParserFlushStale() {
  if (lineLen > 0 && (millis() - lastCharMs) >= LINE_IDLE_FLUSH_MS) {
    processLine();
  }
}
