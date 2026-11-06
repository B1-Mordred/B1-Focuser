# Validation record

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
