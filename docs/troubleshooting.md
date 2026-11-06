# Troubleshooting

| Symptom | Check |
| --- | --- |
| No USB port | Data-capable cable, host enumeration, correct connector, BOOT/RESET sequence |
| Permission denied | Serial group membership and fresh login |
| Port busy | Disconnect INDI; close Arduino Serial Monitor and other clients |
| Driver fails to connect | MoonLite driver, serial transport, correct host-side port, 9600 baud |
| Motor LEDs but no movement | External 5V supply, common ground, connector seating, mechanical load |
| Buzzing or weak motion | GPIO-to-IN mapping, correct AccelStepper ordering, supply sag, excessive speed/load |
| Direction opposite to expected | Verify with tiny moves; correct wiring/pin mapping before recalibrating coordinates |
| Temperature stays zero | Expected placeholder; disable compensation |
| Two step modes shown selected | Earlier driver behavior observed; while idle select the intended mode explicitly and verify GH if needed |
| Position wrong after restart | Saved counter is not physical feedback; re-establish reference |
| Motion creeps after idle | Outputs release after 12 seconds; check mechanical holding capacity |

For read-only protocol diagnosis see [protocol](protocol.md). Stop the active profile before opening the serial port from another application. Do not solve a connection failure by starting multiple INDI servers against one USB device.
