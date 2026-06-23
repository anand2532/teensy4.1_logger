#pragma once

#include <Arduino.h>

bool sdLoggerInit();
bool sdLoggerOpenFiles();
void sdDiagMsg(const char *msg);
bool sdWriteLogLine(const char *text);

uint32_t sdLinesLogged();
uint32_t sdWriteErrors();
