#!/usr/bin/env bash
set -euo pipefail
project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
# Use the same pinned library as the firmware build; no mock motion planner.
accel_src="${ACCELSTEPPER_SRC:-$HOME/Arduino/libraries/AccelStepper/src}"
cd "$project_root"
mkdir -p build
printf '%s  %s\n' \
  d9b5155f0c0f642d8fb920d4f714923d284c78bb0725647708cc9d96df495afb "$accel_src/AccelStepper.cpp" \
  c79899c5be4d7ac9b86e1527807afce9bddb0db5a5693f65c026ccfb9e4d21ad "$accel_src/AccelStepper.h" | sha256sum -c -
g++ -std=c++11 -Wall -Wextra -Werror tests/manual_switch_test.cpp -o build/manual-switch-test
./build/manual-switch-test
g++ -std=c++11 -DARDUINO=10800 -Wall -Wextra -Werror \
  -Itests/host -I"$accel_src" tests/firmware_motion_test.cpp \
  "$accel_src/AccelStepper.cpp" -o build/firmware-motion-test
./build/firmware-motion-test
