# Calibration and everyday use

## First movement

1. Start with the motor uncoupled or the drawtube safely away from its ends.
2. Connect using MoonLite at 9600 baud.
3. Select Half Step while idle. Keep this mode during an imaging session.
4. Establish a meaningful positive position using Sync. Sync only changes the counter; it does not move or locate the focuser.
5. Try a small relative move (for example 20 steps), observe the direction, and return. Increase gradually only if clearance is known.

Do not sync mid-travel to zero if you need inward movement: this firmware clamps command positions to nonnegative values. Choose a positive coordinate with room on both sides, then map the usable range. Keep the range within 0–65535 for conventional MoonLite clients.

## Establish travel

At low speed, measure usable inward and outward travel without running against either stop. Record the step mode, coordinate reference, and limits. Configure corresponding limits in your client, leaving margins for deceleration. Firmware does not enforce a maximum; a client-side setting is not a hardware endstop.

The motor has no encoder. Position is a commanded-step counter. Slippage, missed steps, hand rotation, or load creep invalidate the reference. Re-establish it after any of these events.

## Daily workflow

Connect USB and motor power, load the saved profile, verify mode and position, and make a small test move. Start near focus manually, then tune autofocus step size and backlash using your camera and actual mechanics.

Abort decelerates; it is not an immediate emergency stop. Do not rely on it at a mechanical endstop.

Allow at least five idle seconds after motion before normal power-down so the position can be saved. Coils turn off after approximately twelve idle seconds. Check that your focuser holds the camera load without motor holding torque.

## Step modes

Half step is the default for new Preferences storage. Full step and half step use different coordinate scales (roughly 1:2). Do not switch during a move or autofocus run. If changing mode during setup, recheck the physical reference and all step-based settings, including limits and backlash.

Current conversion rounds odd half-step positions and does not guarantee electrical phase continuity. See [known issues](known-issues.md). The safest established workflow is to stay in half-step mode.
