# Changelog

## 2026-11-19 — staged-target safety fix

- Remove motion arming from idle full/half-step conversion. SN stays staged until FG.
- Add host regression coverage using the actual sketch and AccelStepper 1.64; run it in CI.
- Preserve moving-mode rejection, manual priority, numeric conversion and current-position persistence.
- No hardware validation, phase-continuity improvement or mechanical-accuracy claim.

## Unreleased

- Renamed the project and Arduino sketch to B1-Focuser; updated build, upload, test, and documentation paths.

### 2026-11-06: manual focus switch

- Enabled GPIO0/GPIO1 centre-off switch with internal pull-ups.
- Added debounced starts, prompt centre stop, centre-before-start interlock, and abort inhibition until centre.
- Manual control cancels a computer move and blocks conflicting writes while engaged.
- Added read-only GX contact diagnostic and host state-machine tests.

### 2026-11-02: initial packaging

- Organized existing firmware into an Arduino-compatible firmware directory.
- Added wiring, build, calibration, StellarMate, protocol, and troubleshooting guides.
- Documented historical test results and outstanding limitations.
- Added build/upload scripts, compile workflow, and license notices.

Firmware behavior is unchanged from the earlier full/half-step implementation.
