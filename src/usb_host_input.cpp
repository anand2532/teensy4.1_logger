#include "usb_host_input.h"

#include <USBHost_t36.h>

#include "config.h"
#include "line_parser.h"
#include "sd_logger.h"

static USBHost myusb;
static USBHub hub1(myusb);
static USBHub hub2(myusb);
static USBHIDParser hid1(myusb);
static USBHIDParser hid2(myusb);
static USBHIDParser hid3(myusb);
static USBSerial_BigBuffer userial(myusb, 1);

static USBDriver *drivers[] = {&userial, &hub1, &hub2, &hid1, &hid2, &hid3};
static const char *driverNames[] = {"userial", "hub1", "hub2", "hid1", "hid2", "hid3"};
static constexpr uint8_t DRIVER_COUNT = sizeof(drivers) / sizeof(drivers[0]);
static bool driverActive[DRIVER_COUNT] = {};

static bool sourceReady = false;
static uint32_t rxBytes = 0;
static unsigned long lastDtrMs = 0;

static void onSourceConnected() {
  userial.begin(SERIAL_BAUD);
  userial.setDTR(true);
  userial.setRTS(true);
  lastDtrMs = millis();
  sourceReady = true;

  char buf[80];
  snprintf(buf, sizeof(buf), "USB CDC attached vid=%04x pid=%04x DTR=1",
           userial.idVendor(), userial.idProduct());
  sdDiagMsg(buf);
  Serial.println(buf);
  sdWriteLogLine("--- USB CDC source connected ---");
}

static void onSourceDisconnected() {
  sourceReady = false;
  sdDiagMsg("USB CDC source disconnected");
  Serial.println("USB CDC source disconnected");
  sdWriteLogLine("--- USB CDC source disconnected ---");
}

static void pollDevices() {
  for (uint8_t i = 0; i < DRIVER_COUNT; i++) {
    if (*drivers[i] == driverActive[i]) {
      continue;
    }

    driverActive[i] = *drivers[i];

    char buf[96];
    snprintf(buf, sizeof(buf), "driver %s %s vid=%04x pid=%04x",
             driverNames[i], driverActive[i] ? "ON" : "OFF",
             drivers[i]->idVendor(), drivers[i]->idProduct());
    sdDiagMsg(buf);
    Serial.println(buf);

    if (drivers[i] == &userial) {
      if (driverActive[i]) {
        onSourceConnected();
      } else {
        onSourceDisconnected();
      }
    }
  }
}

static void refreshDtr() {
  if (!sourceReady || (millis() - lastDtrMs) < DTR_REFRESH_MS) {
    return;
  }
  userial.setDTR(true);
  userial.setRTS(true);
  lastDtrMs = millis();
}

static void readInput() {
  while (userial.available() > 0) {
    rxBytes++;
    lineParserOnByte(static_cast<char>(userial.read()));
  }
}

void usbHostInputBegin() {
  myusb.begin();
}

void usbHostInputPoll() {
  myusb.Task();
  pollDevices();
  refreshDtr();
  readInput();
}

bool usbHostSourceReady() {
  return sourceReady;
}

uint32_t usbHostRxBytes() {
  return rxBytes;
}
