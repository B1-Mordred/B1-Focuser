# Hardware and wiring

## Parts

| Part | Requirement |
| --- | --- |
| Controller | ESP32-C3 development board with native USB |
| Motor | 28BYJ-48, **5V** version |
| Driver | ULN2003/ULN2003A breakout with matching five-pin motor socket |
| Motor supply | Regulated 5V; 1A is a practical supply rating |
| USB cable | Data-capable cable to PC or StellarMate |
| Mechanics | Rigid bracket and correctly sized flexible coupler or belt drive |
| Optional capacitor | 470–1000µF, rated above 5V, across driver supply |

## Connections

Disconnect power while wiring.

| From | To |
| --- | --- |
| ESP32 GPIO4 | ULN2003 IN1 |
| ESP32 GPIO5 | ULN2003 IN2 |
| ESP32 GPIO6 | ULN2003 IN3 |
| ESP32 GPIO7 | ULN2003 IN4 |
| ESP32 GND | ULN2003 GND |
| External regulated +5V | ULN2003 VCC / + |
| External supply negative | ULN2003 GND / − |
| Motor five-pin plug | ULN2003 motor socket |
| PC / StellarMate USB | ESP32 USB connector |

```text
PC / StellarMate ─── USB data + power ─── ESP32-C3
                                         GPIO4..7 ─── IN1..4
                                         GND ────── driver GND
Regulated 5V supply:  +5V ────────────────────────── driver VCC
                     GND ────────────────────────── driver GND
                                                       │
                                                   28BYJ-48
```

USB powers the ESP32. The external supply powers the motor. Join grounds, but do not join the external +5V rail to the ESP32 USB/5V rail in this arrangement. Never connect motor power to 3V3 or feed 5V into GPIO inputs. Do not connect a StellarMate 12V output directly to this motor.

If fitting the capacitor, positive goes to driver VCC and negative to GND. The ESP32 signals go to the four inputs, not the motor output pins.

## Manual switch

For the optional manual centre-off switch, connect common to GND and the throws to GPIO0/GPIO1. See [switch wiring and behavior](manual-switch.md).

## Driver jumper

Breakout designs differ. A jumper beside the supply connector is often a motor-power link, but its exact role has not been verified for this board. Confirm the PCB markings or schematic before changing it; do not treat an unidentified jumper as a voltage selector.

## Mechanical assembly

Measure the actual shaft and choose a matching coupler; no bracket CAD or shaft dimensions are supplied. A belt can accommodate alignment and reduction. Mount to the coarse-focus shaft where practical, keeping the bracket rigid without putting side load on the focuser. Confirm free travel and direction with tiny moves before attaching a camera load.

There are no endstop inputs or encoder feedback. The drive releases its coils after about 12 seconds idle; check that the loaded drawtube does not creep.
