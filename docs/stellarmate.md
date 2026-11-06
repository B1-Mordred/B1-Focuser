# StellarMate and Ekos setup

## Connect the hardware

Plug the ESP32 USB into the StellarMate host. Supply the motor with regulated 5V and common ground as described in [hardware](hardware.md).

## Configure the equipment profile

1. Edit your equipment profile in StellarMate or Ekos.
2. Select **MoonLite** for the focuser (`indi_moonlite_focus`).
3. Start the profile and assign the ESP32 port using Port Selector or the INDI Control Panel.
4. Use **Serial**, **9600 baud**, and the board's serial port.
5. Connect and save the driver configuration.

For the general profile workflow, see the official [equipment profiles tutorial](https://stellarmate.com/support/tutorials/equipment-profiles). Menu labels vary by version.

On the StellarMate host, find the port with:

```bash
ls -l /dev/serial/by-id/
```

Prefer the matching persistent path if present. `/dev/ttyACM0` is only an example and may change.

## Desktop Ekos with remote StellarMate

When KStars runs on another computer, select a remote profile using the StellarMate hostname or IP and the INDI server port (normally 7624). The USB port path must refer to the **StellarMate host**, not the desktop. Avoid starting a second server for the same hardware. See [official Ekos setup](https://stellarmate.com/support/ekos/setup).

## Focus configuration

Select MoonLite in the imaging optical train and select that train in the Focus module. Choose Half Step while idle, leave temperature compensation disabled, and begin with zero backlash compensation until measured. Follow [calibration](usage.md) before choosing a travel limit or starting autofocus.

The stock driver can display temperature and other controls that the hardware does not implement. A displayed zero temperature is a placeholder, not a measurement. Mode switching changes position units and has [known limitations](known-issues.md).

Start with a small manual movement, confirm the physical direction, then return to the starting position. Once travel, load, and backlash are characterized, tune autofocus step size using star images. There is no universal safe step size or maximum for an unmeasured bracket/coupler.

The earlier real INDI backend tests passed; StellarMate GUI operation and on-sky autofocus have not been independently tested for this package.
