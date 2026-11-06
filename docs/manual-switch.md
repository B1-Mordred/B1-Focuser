# Manual focus switch

Connect the common of one pole of a centre-off switch to ESP32 GND. Connect its two throws to GPIO0 and GPIO1. Leave the other pole of a DPDT switch unused. Use a continuity meter to identify contacts. Neither input should connect to GND in the centre position. No 5V connection is used.

| Contact | Firmware action |
| --- | --- |
| GPIO0 grounded | Decrease position toward zero |
| GPIO1 grounded | Increase position toward 65535 |
| Centre, both open | Stop manual steps |
| Both grounded | Stop and require centre before rearming |

Internal pull-ups are enabled. A spring-return (ON)-OFF-(ON) switch stops when released; a latching ON-OFF-ON switch must be returned to centre manually.

After power-up or serial abort, the switch must be centred for at least 30 ms before manual movement is enabled. Starts and reversals are debounced for 30 ms. Leaving the active contact stops manual stepping on the next loop iteration, without a programmed deceleration tail. This does not guarantee the mechanism stops without mechanical settling or missed steps under every load.

Manual speed is 120 steps/second, with acceleration 120 steps/second². On centre, the configured serial speed is restored. A manual start cancels any previous computer move; that move does not resume later. Queries and abort remain available, but motion, sync, speed and mode writes are ignored while the switch is engaged or awaiting centre. MoonLite writes have no error acknowledgement, so do not operate the switch during autofocus; stop the autofocus run first.

The position counter is shared with INDI and saved after five seconds idle. GPIO0 cannot decrease below zero. GPIO1 does not increase above 65535; these are numeric bounds, **not calibrated mechanical travel limits**. Confirm clearance and actual direction with tiny movements before labelling the lever IN and OUT. At position zero, only GPIO1 will produce movement.

## Read-only wiring check

The project-specific `:GX#` query reports raw input contacts: `00#` centre, `01#` GPIO0 grounded, `02#` GPIO1 grounded, `03#` both grounded. This is not a standard MoonLite command. With motor power disconnected, use it to check each lever position; do not turn motor power on until the switch is centred again.

## Host logic test

From the project root:

```bash
mkdir -p build
g++ -std=c++11 -Wall -Wextra -Werror tests/manual_switch_test.cpp -o build/manual-switch-test
./build/manual-switch-test
```

This tests the real switch state machine. Physical contacts, motor behavior, and loaded stopping still need hardware verification.
