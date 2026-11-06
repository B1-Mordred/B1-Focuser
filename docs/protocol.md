# Implemented MoonLite protocol

Serial: 9600 baud; commands start with `:` and end with `#`. Queries reply with a hash terminator. Set commands generally have no acknowledgement. Numeric parameters and responses are hexadecimal. INDI property values use decimal.

| Command | Current behavior |
| --- | --- |
| GV | Firmware identifier `10#` |
| GP / GN | Current / staged target position |
| GI | `01#` moving, `00#` idle |
| SPxxxx | Sync counter immediately; no physical homing |
| SNxxxx | Stage absolute target |
| FG | Start move to staged target |
| FQ | Decelerate to stop |
| GD / SDxx | Get / set speed divisor; clamped to 02–20 hex |
| GH | `FF#` half step, `00#` full step |
| SH / SF | Select half / full step while idle |
| PH | Move to logical zero; **not sensor-based homing** |
| GT | Placeholder `0000#`; no temperature measurement |
| GB / GC | Placeholder `00#` |
| GX | Project-specific raw switch inputs: 00 centre, 01 GPIO0, 02 GPIO1, 03 both grounded |

While the manual switch is engaged or waiting for centre after startup/abort, SP, SN, FG, PH, SH, SF and SD writes are ignored. Queries and FQ remain available. FQ stops manual stepping immediately and requires centre before another manual start. Normal computer moves retain their existing decelerating abort behavior.

Speed is 960 divided by the divisor in steps/second; acceleration is 120 steps/second². Actual loaded motion is not guaranteed at all rates. The same step rate in full mode produces about twice the physical speed.

## Read-only queries

With INDI disconnected, send one command at a time using a serial terminal at 9600 baud (no line ending):

```text
:GV#
:GP#
:GI#
:GH#
```

Expected firmware identifier: `10#`. These queries do not command motor movement, although opening a serial connection may reset some boards.

For movement use the calibrated INDI controls described in [usage](usage.md); do not copy arbitrary absolute targets onto a mounted focuser. Parser validation is minimal, and malformed commands are not robustly rejected.
