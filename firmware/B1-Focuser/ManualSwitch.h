#pragma once
#include <stdint.h>

// B1-Focuser input bits: 1 = IN grounded, 2 = OUT grounded, 0 = centre.
// Starting requires a stable input; leaving a direction stops immediately.
class ManualSwitch
{
public:
  enum Action { NONE, STOP, IN, OUT };
  static constexpr uint32_t DEBOUNCE_MS = 30;

  Action update(uint8_t input, uint32_t now)
  {
    const bool changed = !initialized || input != candidate;
    if (changed)
    {
      initialized = true;
      candidate = input;
      changedAt = now;
    }
    if (input == 3)
    {
      armed = false;
      active = 0;
      if (changed) return STOP;
    }
    if (active != 0 && input != active)
    {
      active = 0;
      return STOP;
    }
    if (uint32_t(now - changedAt) < DEBOUNCE_MS) return NONE;
    if (input == 0)
    {
      armed = true;
      return NONE;
    }
    if (!armed || input == 3 || active == input) return NONE;
    active = input;
    return input == 1 ? IN : OUT;
  }

  void inhibit(uint32_t now)
  {
    active = 0;
    armed = false;
    changedAt = now;
  }

  bool blocksRemoteMotion() const { return !armed || candidate != 0 || active != 0; }
  bool isActive() const { return active != 0; }

private:
  bool initialized = false;
  bool armed = false;
  uint8_t candidate = 0;
  uint8_t active = 0;
  uint32_t changedAt = 0;
};
