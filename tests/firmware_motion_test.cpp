// Execute the actual sketch/parser/loop and AccelStepper 1.64 on a host.
// Only Arduino time, GPIO, serial and Preferences are simulated; no hardware IO.
#include "Arduino.h"
#include <cassert>
#include <iostream>
uint32_t hostMs = 0;
int hostPins[32];
HostSerial Serial;
#include "../firmware/B1-Focuser/B1-Focuser.ino"

void tick(unsigned ms = 1) { hostMs += ms; loop(); }
void send(const std::string &cmd) {
  Serial.input = ":" + cmd + "#";
  tick();
  assert(Serial.input.empty());
}
void query(const std::string &cmd, const std::string &expected) {
  Serial.output.clear(); send(cmd); assert(Serial.output == expected);
}
void idle(unsigned ms = 100) { for (unsigned i = 0; i < ms; ++i) tick(); }
void finish() {
  for (unsigned i = 0; activeStepper().distanceToGo() && i < 20000; ++i) tick();
  assert(activeStepper().distanceToGo() == 0);
}
void reset(StepMode mode, long pos, bool enabled = false) {
  disableMotor();
  fullStepper.setCurrentPosition(0); halfStepper.setCurrentPosition(0);
  stepMode = mode; activeStepper().setCurrentPosition(pos);
  targetPosition = lastSavedPosition = pos; lastSavedStepMode = mode;
  manualSwitch = ManualSwitch(); hostPins[0] = hostPins[1] = HIGH;
  handleManualButtons(); hostMs += 30; handleManualButtons();
  updateSpeed(); if (enabled) enableMotor();
}
int main() {
  std::fill(hostPins, hostPins + 32, HIGH);
  setup(); idle(31);
  // Both directions, retained/released outputs, odd/even coordinates,
  // repeated switches and reboot: SN never arms motion.
  for (auto mode : {STEP_HALF, STEP_FULL})
    for (bool enabled : {false, true})
      for (long pos : {100L, 101L}) {
        reset(mode, pos, enabled);
        send("SN0120");
        query("GI", "00#"); idle(); assert(activeStepper().currentPosition() == pos);
        const auto other = mode == STEP_HALF ? STEP_FULL : STEP_HALF;
        send(other == STEP_HALF ? "SH" : "SF");
        const long converted = fromHalfStepUnits(toHalfStepUnits(pos, mode), other);
        const long staged = fromHalfStepUnits(toHalfStepUnits(0x120, mode), other);
        assert(activeStepper().currentPosition() == converted);
        assert(targetPosition == staged && outputsEnabled == enabled);
        query("GI", "00#");
        char hex[16]; snprintf(hex, sizeof(hex), "%04lX#", staged); query("GN", hex);
        idle(); assert(activeStepper().currentPosition() == converted);
        assert(prefs.getLong("pos", -1) == converted);
        assert(prefs.getBool("half", false) == (other == STEP_HALF));
        send(other == STEP_HALF ? "SH" : "SF"); // same-mode no-op
        send(mode == STEP_HALF ? "SH" : "SF");
        send(other == STEP_HALF ? "SH" : "SF");
        assert(activeStepper().distanceToGo() == 0);
        send("FG"); assert(activeStepper().distanceToGo() != 0);
        finish(); assert(activeStepper().currentPosition() == staged);
        idle(5001); assert(prefs.getLong("pos", -1) == staged);
        // Reboot restores current position/mode, not an SN target.
        send("SN0200"); setup();
        assert(stepMode == other && targetPosition == staged);
        idle(); assert(activeStepper().currentPosition() == staged);
      }
  reset(STEP_HALF, 100);
  send("SN0200"); send("FG"); idle(200);
  const long movingTarget = activeStepper().targetPosition();
  send("SN0300"); send("SF");
  assert(stepMode == STEP_HALF && activeStepper().targetPosition() == movingTarget);
  finish(); assert(activeStepper().currentPosition() == movingTarget);
  send("SF"); idle(); assert(activeStepper().distanceToGo() == 0);
  send("FG"); finish(); assert(activeStepper().currentPosition() == 0x180);
  // Abort decelerates, rejects switching until stopped, and cannot resurrect SN.
  reset(STEP_HALF, 100); send("SN0300"); send("FG"); idle(300);
  send("FQ"); send("SF"); assert(stepMode == STEP_HALF);
  finish(); idle(31); send("SF"); idle(); assert(activeStepper().distanceToGo() == 0);
  // Manual takeover cancels staged/active targets; writes remain blocked.
  for (bool running : {false, true}) {
    reset(STEP_HALF, 100); send("SN0200"); if (running) send("FG");
    hostPins[1] = LOW; tick(); idle(30);
    assert(manualSwitch.isActive());
    const long manualTarget = targetPosition;
    send("SF"); send("SN0300"); send("FG");
    assert(stepMode == STEP_HALF && targetPosition == manualTarget);
    hostPins[1] = HIGH; tick(); const long stopped = activeStepper().currentPosition();
    idle(31); send("SF"); idle();
    assert(activeStepper().currentPosition() == (stopped + 1) / 2);
    assert(activeStepper().distanceToGo() == 0);
  }
  // Explicit sync and logical-home retain their existing semantics.
  reset(STEP_HALF, 100); send("SN0200"); send("SP0040"); send("SF");
  idle(); assert(targetPosition == 32 && activeStepper().currentPosition() == 32);
  send("PH"); finish(); assert(activeStepper().currentPosition() == 0);
  std::cout << "PASS: staged targets, modes, FG, active motion, abort, manual takeover, persistence, sync, logical home\n";
}
