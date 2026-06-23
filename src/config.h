#pragma once

#include <Arduino.h>

constexpr char LOG_DIR[] = "logs";
constexpr char DATA_LOG[] = "logs/data.log";
constexpr char DIAG_LOG[] = "logs/diag.log";

constexpr size_t LINE_BUF_SIZE = 512;
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t LINE_IDLE_FLUSH_MS = 100;
constexpr uint32_t DIAG_INTERVAL_MS = 10000;
constexpr uint32_t DTR_REFRESH_MS = 2000;
constexpr uint8_t SD_RETRY_COUNT = 8;
