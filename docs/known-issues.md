# Known issues and unsupported features

This package preserves the existing firmware. The following are source-review findings, not newly measured hardware failures.

## Step-mode switching

- Separate full-step and half-step AccelStepper objects do not preserve electrical phase through the current coordinate conversion. Switching or syncing may cause a small phase discontinuity on subsequent motion.
- Odd half-step positions round when converted to full steps; repeated toggling can change coordinates.
- Switching mode after staging a target with SN can start movement without a subsequent FG because the mode-switch routine calls moveTo.
- A mode change while moving is silently ignored. A driver may display its requested state without confirming the actual mode.

Use a fixed half-step mode for normal operation. Phase-preserving motion and robust mode-change semantics remain follow-up work.

## Limits and persistence

No physical homing, encoder, endstops, or firmware maximum travel limit. PH moves to logical zero only. Abort decelerates. Position and mode are saved as separate Preferences writes; interruption can leave inconsistent or stale values. Speed is not persisted.

Position responses can exceed the conventional four-digit MoonLite range; keep configured coordinates within 0–65535. Minimal parsing does not reject all malformed or oversized commands.

## Controls

Temperature, temperature compensation, and onboard backlash compensation are absent. Stock INDI MoonLite still exposes properties for unsupported functions. A tailored INDI driver is needed to remove those controls.

The speed divisor clamps to 2–32; a driver request for divisor 1 will be clamped. Not every displayed speed selection necessarily maps distinctly.

Manual switch control is enabled on GPIO0/GPIO1. Returning to centre stops manual stepping, and startup/abort require centre before rearming. Remote writes are ignored while manual control is engaged; the MoonLite protocol provides no error acknowledgement for these writes. See [manual switch](manual-switch.md).

## Mechanics

Software counters do not establish real positioning accuracy. Gearbox backlash, coupler slip, missed steps, and drawtube creep remain unmeasured. Outputs release after twelve seconds idle; there is no active holding torque afterward.
