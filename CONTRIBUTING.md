# Contributing

Read docs/known-issues.md before changing movement behavior. Build with ./scripts/build.sh and describe the board, driver, power supply, and dependency versions used for any hardware test.

Keep serial debug messages off the MoonLite command channel. Separate compile success, protocol results, INDI backend results, GUI testing, and physical measurements in reports.

Motion tests should use a known safe range, an observed motor, and a recoverable coordinate reference. Do not treat a reported step count as proof that the mechanism moved accurately.

Priorities include phase-preserving stepping, enforced travel limits, robust parsing, atomic persistence, and a driver that exposes only supported controls.
