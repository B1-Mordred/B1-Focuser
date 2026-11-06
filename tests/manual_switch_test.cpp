#include "../firmware/sv48p-esp32c3-moonlite-focuser/ManualSwitch.h"
#include <cassert>
#include <iostream>

int main()
{
  using S = ManualSwitch;
  S s;
  // Held at boot must never start until a stable centre is observed.
  assert(s.update(1, 0) == S::NONE);
  assert(s.update(1, 1000) == S::NONE);
  assert(s.blocksRemoteMotion());
  assert(s.update(0, 1001) == S::NONE);
  assert(s.update(0, 1030) == S::NONE);
  assert(s.blocksRemoteMotion());
  assert(s.update(0, 1031) == S::NONE);
  assert(!s.blocksRemoteMotion());
  assert(s.update(1, 1040) == S::NONE);
  assert(s.blocksRemoteMotion());
  assert(s.update(1, 1069) == S::NONE);
  assert(s.update(1, 1070) == S::IN);
  assert(s.update(1, 2000) == S::NONE);
  // Release stops on first sample. Bounce cannot restart immediately.
  assert(s.update(0, 2001) == S::STOP);
  assert(s.update(1, 2002) == S::NONE);
  assert(s.update(0, 2003) == S::NONE);
  assert(s.update(0, 2033) == S::NONE);
  assert(!s.blocksRemoteMotion());
  assert(s.update(2, 2040) == S::NONE);
  assert(s.update(2, 2070) == S::OUT);
  // Direct reversal stops first, then requires a stable new direction.
  assert(s.update(1, 2071) == S::STOP);
  assert(s.update(1, 2100) == S::NONE);
  assert(s.update(1, 2101) == S::IN);
  // Abort cannot be undone by a held switch.
  s.inhibit(2110);
  assert(s.update(1, 9000) == S::NONE);
  assert(s.blocksRemoteMotion());
  s.update(0, 9001);
  s.update(0, 9031);
  s.update(2, 9040);
  assert(s.update(2, 9070) == S::OUT);
  // Both grounded is a fault: stop and require centre to rearm.
  assert(s.update(3, 9071) == S::STOP);
  assert(s.update(1, 9080) == S::NONE);
  assert(s.update(1, 9200) == S::NONE);
  s.update(0, 9201);
  s.update(0, 9231);
  assert(!s.blocksRemoteMotion());
  // Centre never produces STOP for an ordinary computer-controlled move.
  assert(s.update(0, 10000) == S::NONE);
  // A wiring fault also cancels computer motion when manual input was idle.
  assert(s.update(3, 10001) == S::STOP);
  assert(s.update(3, 10002) == S::NONE);
  S wrap;
  wrap.update(0, UINT32_MAX - 10);
  wrap.update(0, 20);
  assert(!wrap.blocksRemoteMotion());
  std::cout << "PASS: startup, debounce, release, reversal, abort, dual-input fault, remote idle, timer wrap\n";
}
