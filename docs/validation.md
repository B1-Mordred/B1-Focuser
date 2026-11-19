# Validation record

## Step-mode staged-target fix (2026-11-19)

Base reviewed: `414f930ee94bca4ea73fa7062be9546e0dfee317`.
The only firmware behavior change removes `newStepper.moveTo(targetPosition)`
from `setStepMode()`. AccelStepper's `setCurrentPosition()` resets its internal
target and velocity. The separately converted SN target remains available to GN
and FG. Mode changes during a move (including FQ deceleration) remain ignored;
clients must read GH. SN during a move stages a future target without retargeting
the active move. PH and manual input intentionally remain movement initiators.
Manual takeover/release continues to replace the staged target with the manual
motion/current position, and remote writes remain blocked while engaged.

Run `bash scripts/test-host.sh` after installing AccelStepper 1.64, or set
`ACCELSTEPPER_SRC` to its `src` directory. The runner checks source hashes and
compiles the actual sketch plus the real library with `-Wall -Wextra -Werror`.
Arduino time, serial, GPIO and Preferences are host adapters; they do not access
hardware. The tests exercise framed commands through `readSerialCommands()` and
`loop()`, rather than a duplicate implementation of the motion logic.

Host regressions passed: SN → SF/SH → idle → FG in both directions, odd/even
positions, outputs enabled/disabled, same/repeated mode commands, GN/GI replies,
SN during an active move, mode rejection during movement/abort, manual takeover
from staged/active motion and release, SP sync, PH, current-position/mode saves
and simulated setup after restart. The original firmware fails the staged-target
idle assertion with the same test harness. Existing manual-switch tests pass.

Persistence still records the converted **current** position and mode, never the
staged target; the patch preserves upward rounding of odd half steps. Preferences
writes are simulated, so flash endurance and interrupted writes are untested.
Separate writes remain non-atomic. Counter rounding can accumulate on toggles.
Switching disables coil outputs and does not preserve electrical phase; a rotor
may settle or shift even without a queued software move. Host counters establish
neither electrical phase continuity nor mechanical positioning accuracy, backlash,
slip, missed-step behavior or safe travel. Fixed half-step operation remains
recommended until separately authorized hardware validation.

No device connection, upload or real motor movement was performed for this patch.

## Manual switch update (2026-11-06)

- Host tests passed for startup with a held switch, debounce, release, reversal, serial-abort inhibition, both-input fault, remote idle, and timer wrap.
- Final firmware compiled with ESP32 core 3.3.12 and uploaded successfully to the user-confirmed ESP32-C3; flash hashes verified.
- Read-only checks after upload: GV=10, GP=0000, GI=00 (idle), GH=FF (half-step), GX=00 (both switch inputs open).
- Both lever directions and physical motor stopping have not yet been tested with the user. No motor move was commanded during this update's hardware checks.
- This update supersedes the earlier disabled optional-button behavior. Source is no longer identical to the original packaging snapshot.

## Earlier hardware sessions (2026-10-04)

Historical session records report:
- ESP32-C3 compiled and flashed with Arduino ESP32 core 3.3.11 and AccelStepper 1.64.
- Direct serial handshake, sync, status, bounded moves and return.
- A 2048-half-step out-and-back motion test.
- Real indi_moonlite_focus backend: absolute 0→800→0, relative +400/−400, abort with deceleration, and speed property changes.
- Full/half mode selection via serial and INDI, with GH readback; numeric conversion 64 full steps→128 half steps.
- Board left at half-step and logical zero at that time.

These are historical results, not a statement about the currently attached device or its current firmware. Observing counters reach targets does not measure physical accuracy.

## Packaging validation (2026-11-02)

The sketch was copied unchanged from the existing project. This packaging task checks compilation with the locally installed ESP32 core 3.3.12 and AccelStepper 1.64, shell syntax, repository links, and source identity. See the build log in local ignored build/ when present.

No flashing or motor movement is performed by packaging or CI. The GitHub workflow compiles firmware only.

Packaging checks passed: compilation (312778 bytes flash, 14740 bytes RAM), shell syntax, local Markdown links, and byte-for-byte firmware source identity. The local compile output is saved in ignored `build/compile.log`. The GitHub Actions workflow has not yet run on GitHub.

## Not established

- Electrical phase continuity during mode switching or sync
- Travel limits, repeatability, backlash, or holding behavior under camera load
- Encoder-based positioning accuracy
- Power-loss recovery and persistence under interruption
- Full parser robustness
- Manual StellarMate/Ekos GUI testing and on-sky autofocus
- ASCOM compatibility testing

Read [known issues](known-issues.md) before extending the firmware.
