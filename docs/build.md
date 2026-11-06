# Build and flash

## Tested configuration

- Arduino CLI 1.2.2
- Arduino ESP32 core 3.3.12 for this packaged build
- AccelStepper 1.64
- FQBN: `esp32:esp32:esp32c3:CDCOnBoot=cdc`

Earlier hardware flashing used core 3.3.11. The scripts and CI use the same board options. Preferences is included with the ESP32 core.

## Install dependencies

Install [Arduino CLI](https://arduino.github.io/arduino-cli/latest/installation/), then:

```bash
arduino-cli core update-index --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli core install esp32:esp32@3.3.12 --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli lib install "AccelStepper@1.64"
```

## Compile

Run from the repository root:

```bash
./scripts/build.sh
```

Build products go in ignored `build/`. The scripts also work when invoked from another directory.

## Flash

Disconnect the focuser in INDI/Ekos and close serial monitors first.

```bash
arduino-cli board list
ls -l /dev/serial/by-id/
./scripts/flash.sh /dev/ttyACM0
```

Replace the example port with the board you identified. Flashing compiles first, then uploads. On Linux, if serial access is denied, add your account to the appropriate serial group (commonly `dialout`) and log out/in:

```bash
sudo usermod -aG dialout "$USER"
```

For a C3 stuck outside download mode, hold BOOT, tap RESET, release BOOT, then retry the upload. Recheck the port after resetting.

## Arduino IDE

1. Add the Espressif package-index URL above in Additional Boards Manager URLs.
2. Install **esp32 by Espressif Systems**, version 3.3.12, and **AccelStepper**, version 1.64.
3. Open `firmware/sv48p-esp32c3-moonlite-focuser/sv48p-esp32c3-moonlite-focuser.ino`.
4. Select **ESP32C3 Dev Module**, **USB CDC On Boot: Enabled**, and the correct port.
5. Verify, then Upload.

These settings target native USB. Boards with a separate USB-to-UART bridge may need different CDC settings and have not been validated here.

## Source configuration

GPIO assignments are near the top of the sketch. AccelStepper's constructor intentionally uses IN1, IN3, IN2, IN4 order. The centre-off manual switch uses GPIO0/GPIO1 with internal pull-ups; see [manual switch](manual-switch.md).

Uploading normally retains Preferences data. After a reset, verify the displayed position against the physical focuser before issuing a move.
