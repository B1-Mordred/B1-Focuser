# B1-Focuser

USB motorized focuser for an **SVBONY SV48P**, using an **ESP32-C3**, **28BYJ-48 5V motor**, and **ULN2003 driver**. Speaks the MoonLite serial protocol for INDI, Ekos, and StellarMate.

[GitHub repository](https://github.com/B1-Mordred/B1-Focuser)

**Status:** working prototype. Earlier hardware tests verified serial commands and the INDI backend. Physical positioning accuracy and on-sky autofocus have not been measured. See [validation](docs/validation.md) and [known limitations](docs/known-issues.md).

## Start here

1. [Parts, mounting, and power wiring](docs/hardware.md)
2. [Build and flash the firmware](docs/build.md)
3. [Set up StellarMate / Ekos](docs/stellarmate.md)
4. [First use and calibration](docs/usage.md)
5. [Troubleshooting](docs/troubleshooting.md)
6. [Manual focus switch on GPIO0/GPIO1](docs/manual-switch.md)

## Repository layout

```text
firmware/B1-Focuser/
  B1-Focuser.ino
  ManualSwitch.h
docs/
  hardware.md           Parts, wiring, power, mounting
  build.md              Arduino CLI and IDE instructions
  stellarmate.md         StellarMate and desktop Ekos
  usage.md              Calibration and daily operation
  protocol.md           Implemented serial commands
  troubleshooting.md    Connection and motion problems
  manual-switch.md      GPIO0/GPIO1 centre-off switch
  validation.md         Evidence and test boundaries
  known-issues.md        Firmware limitations and follow-up work
  publishing.md         Publishing this folder to GitHub
scripts/
  build.sh              Compile into ignored build/
  flash.sh              Upload to an explicitly supplied port
tests/
  manual_switch_test.cpp  Manual-control state-machine tests
.github/workflows/
  build.yml             Firmware compile check
licenses/               Upstream attribution
LICENSE                 GPL-3.0 license text
THIRD_PARTY_NOTICES.md
CHANGELOG.md
CONTRIBUTING.md
```

## Quick build

After installing the dependencies in [build instructions](docs/build.md):

```bash
./scripts/build.sh
./scripts/flash.sh /dev/ttyACM0
```

Select **MoonLite** (driver executable `indi_moonlite_focus`), serial, **9600 baud**. Use Half Step for normal operation. Temperature compensation is unsupported.

## Credits and licensing

Adapted from the protocol behavior of [fehlfarbe/arduino-motorfocus](https://github.com/fehlfarbe/arduino-motorfocus). Uses AccelStepper and the Arduino ESP32 core.

This project is distributed under **GPL-3.0-or-later**; see [LICENSE](LICENSE) and [third-party notices](THIRD_PARTY_NOTICES.md). Upstream MIT attribution is retained separately.
