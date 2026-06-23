# Teensy 4.1 Logger

PlatformIO project for Teensy 4.1. Reads incoming text from a USB serial device connected to the Teensy 4.1 **USB host port** at 115200 baud, and logs each line to the onboard SD card.

## Wiring (important)

Your `diag.log` shows `usb_ready=0` and all byte counts at zero. That means **no data is reaching the Teensy**. SD logging works; the input connection is wrong.

### Wrong (looks like laptop setup, but does not work on Teensy)

```
Source USB  --->  Teensy micro USB (programming port)   [DOES NOT WORK]
```

The micro USB port is a USB **device** port (like your laptop). It cannot accept another USB serial device the way your laptop does with `minicom /dev/ttyACM0`.

### Correct connection

```
Source USB  --->  Teensy 4.1 USB HOST port (bottom of board)
```

When this works, `diag.log` will show `driver userial ON` and `usb_ready=1`.

## Requirements

- [PlatformIO](https://platformio.org/) (CLI or VS Code / Cursor extension)
- Teensy 4.1 with microSD card (FAT32)
- Source device with USB serial output at 115200 baud

## One-time Linux setup

Teensy upload needs USB access. Install the PJRC udev rules once:

```bash
wget https://www.pjrc.com/teensy/00-teensy.rules -O /tmp/00-teensy.rules
sudo cp /tmp/00-teensy.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
```

Unplug and replug the Teensy after installing the rules.

## Project layout

```
teensy4.1_logger/
├── platformio.ini
├── include/                  # headers (.h)
│   ├── config.h
│   ├── sd_logger.h
│   ├── line_parser.h
│   ├── usb_host_input.h
│   ├── led_status.h
│   └── diagnostics.h
├── src/                      # source (.cpp)
│   ├── main.cpp
│   ├── sd_logger.cpp
│   ├── line_parser.cpp
│   ├── usb_host_input.cpp
│   ├── led_status.cpp
│   └── diagnostics.cpp
└── .pio/                     # build output (generated)
```

## Build

Compile the firmware without uploading:

```bash
pio run
```

Verbose build output:

```bash
pio run -v
```

## Flash (upload)

Connect the Teensy over USB, then:

```bash
pio run -t upload
```

If upload waits for the device, press the **program button** on the Teensy once.

Build and upload in one step (default when no target is given for upload):

```bash
pio run -t upload
```

## Serial monitor

View live output at 115200 baud:

```bash
pio device monitor
```

Exit the monitor with `Ctrl+C`.

Expected output on success:

```
USB host logger ready, logging to logs/sess_0001.csv
Connect source USB serial device to Teensy 4.1 USB host port
USB source connected
```

Status messages only. Incoming log lines from the USB host port are written to the SD card.

## Clean build artifacts

Remove compiled object files and firmware binaries from `.pio/build/`:

```bash
pio run -t clean
```

After cleaning, the next `pio run` or `pio run -t upload` performs a full rebuild.

To remove all PlatformIO-generated project data (build cache, library installs for this project):

```bash
rm -rf .pio
```

The firmware already on the Teensy is **not** erased by cleaning; only local build files are removed.

## SD card logs

On boot the firmware writes to two files in `logs/`:

| File | Purpose |
|------|---------|
| `logs/data.log` | All received serial lines: `[millis] text` |
| `logs/diag.log` | Boot status, USB connect/disconnect, byte counters every 10s |

Example `data.log`:

```
[1523] === LOGGER STARTED ===
[4102] Entering MAIN loop... [PROCESS MODE]
[4200] [WARNING] : [DB] SD card not ready (attempt 1/5)
```

Example `diag.log`:

```
[500] === LOGGER BOOT ===
[501] SD card mounted OK
[12000] stats usb=4096 lines=42 usb_ready=1 sd_err=0
```

### LED indicators

| LED pattern | Meaning |
|-------------|---------|
| 3 blinks at boot | SD card OK, logging started |
| Fast blink | Serial data is being received |
| Solid ~180 ms | Line was written to `data.log` |
| Repeating 1 blink | SD init failed |
| Repeating 3 blinks | SD write test failed |

### Extract logs

1. Power off or stop logging
2. Remove the microSD card
3. Copy files from `logs/` to your computer

Open the CSV in a spreadsheet or with Python/pandas.

### Clear SD card logs

Delete the `logs/` folder (or individual `sess_*.csv` files) on the card from your computer. The firmware will recreate `logs/` on the next boot.

## Troubleshooting

| Problem | Fix |
|---------|-----|
| `SD card init failed` | Use FAT32; reseat the card; try another card |
| `USB source connected` never appears | Plug source into Teensy **host** port; check cable/power |
| SD file empty except boot line | Source not reaching USB host port — check wiring and 5V |
| `diag.log` shows `usb=0` | No data from USB host — check cable and source power |
| `usb_ready=0` in diag | Source not detected on USB host port |
| Upload permission / HID errors | Install udev rules (see above); replug USB |
| Upload waits forever | Press the Teensy program button during upload |
| No serial output on PC | Close other programs using the Teensy USB port; run `pio device monitor` |

## Common commands

| Task | Command |
|------|---------|
| Build | `pio run` |
| Flash | `pio run -t upload` |
| Serial monitor | `pio device monitor` |
| Clean build | `pio run -t clean` |
| List USB devices | `pio device list` |
