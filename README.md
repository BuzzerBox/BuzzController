# BuzzController

Firmware and hardware designs for the BuzzerBox — a 10-button quiz buzzer controller. A Raspberry Pi CM4 acts as the host, communicating with an ATmega328P co-processor over serial. The ATmega handles button detection, locks the active buzzer, and drives a 7-segment display.

---

## Features

- **10 buzzers** with individual LED feedback, interrupt-driven detection
- **7-segment display** showing the active buzzer number and RPi connection status
- **Serial communication** with a Raspberry Pi at 115200 baud, including heartbeat monitoring
- **Soft release** — RPi can remotely unlock the selected buzzer
- **Watchdog timer** for automatic recovery from firmware hangs
- **I2C I/O expanders** (MCP23017) to drive LEDs and display

---

## Repository Structure

```
BuzzController/
├── cad/
│   └── BuzzerBox/
│       ├── controller.brd/.sch    # Main ATmega328P controller board
│       ├── xlrpcb.brd/.sch        # XLR connector board (V3)
│       ├── devkeys.brd/.sch       # Development key PCB
│       ├── ttl_adapter.brd/.sch   # TTL adapter for devkit
│       └── eagle.epf              # EAGLE CAD project file
│   └── BuzzerBoxPanel.f3d         # Fusion 360 enclosure design
└── code/
    ├── platformio.ini             # Build configuration
    └── src/
        ├── BuzzerBox.cpp          # Main firmware logic
        ├── i2c_communication.*    # MCP23017 I/O expander driver
        ├── rpi_communication.*    # Serial protocol with Raspberry Pi
        ├── 7segment.*             # 7-segment display driver
        └── portConfig.hpp         # Pin/port and I2C address configuration
```

---

## Hardware

| Component | Details |
|---|---|
| Host | Raspberry Pi CM4 (mounted directly on PCB) |
| Microcontroller | ATmega328P |
| I/O Expanders | 2× MCP23017 (I2C addresses `0x20`, `0x21`) |
| Display | 7-segment LED (common cathode, via expander) |
| Buttons | 10 buzzers across MCU ports B, C, D + 1 release button |
| CM4 ↔ MCU | Serial UART (115200 baud) |
| Programming | STK500v2 via USB |

PCB designs are in [EAGLE](https://www.autodesk.com/products/eagle/overview) format. The enclosure panel is a [Fusion 360](https://www.autodesk.com/products/fusion-360/overview) file.

---

## Serial Protocol

Communication between the ATmega328P and the Raspberry Pi uses 3-byte UART messages at **115200 baud**:

```
[Command Byte] [Data Byte] [0xFF]
```

### Commands (RPi → Controller)

| Command | Value | Description |
|---|---|---|
| `EMPTY` | `0x00` | No operation |
| `SOFT_RELEASE` | `0x01` | Unlock the active buzzer |
| `SET_BUZZER` | `0x02` | Lock a specific buzzer (data = buzzer ID 0–9) |

### Events (Controller → RPi)

| Event | Byte | Description |
|---|---|---|
| Button pressed | `'0'`–`'9'` (ASCII) | Buzzer ID of the pressed button |
| Buzzer unlocked | `'q'` | Acknowledges an unlock |

A heartbeat is expected from the RPi every **6 seconds**. If no heartbeat is received within this window, the controller transitions to a disconnected state and reflects this on the display's decimal point.

---

## Building and Flashing

The firmware uses [PlatformIO](https://platformio.org/).

```bash
# Install PlatformIO CLI or use the VSCode extension

# Build
pio run

# Upload
pio run --target upload
```

Upload uses STK500v2 at 19200 baud over USB.

---

## Development

The `devkeys` PCB and `ttl_adapter` PCB are development accessories for testing the firmware without the full buzzer assembly.
