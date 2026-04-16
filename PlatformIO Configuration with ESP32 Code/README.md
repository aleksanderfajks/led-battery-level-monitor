# LED Battery Level Monitor

This project runs on an ESP32 and lights a bank of ten LEDs according to a
battery percentage value provided over the serial port. You can use this
with your laptop by sending the current battery level from the host machine
to the ESP32.

## Wiring

Connect ten LEDs (with appropriate current‑limiting resistors) to the
following GPIO pins, in the order shown below:

```
4, 13, 18, 19, 21, 25, 26, 27, 32, 33
```

The first pin in the list is treated as the "lowest" segment of the bar;
the last pin is the highest. 0 % lights none of the LEDs, 100 % lights all
ten of them.

## Protocol

Send a decimal integer between `0` and `100` terminated by a newline (`\n` or
`\r`). You may also append a charging flag separated by a comma – send
`1` when the battery is charging, `0` otherwise. Examples:

```
75            // just percentage
42,1          // 42% and currently charging
```

When charging is indicated the ESP32 will briefly "fill" the bar by lighting
the remaining LEDs one by one, then return to showing the actual percentage.
The value (and charging state) is echoed back on the serial port.

### Example Python sender

```python
import psutil       # pip install psutil
import serial        # pip install pyserial
import time

ser = serial.Serial('COM3', 115200, timeout=1)  # adjust port

while True:
    battery = psutil.sensors_battery()
    if battery is None:
        break
    pct = int(battery.percent)
    charging = 1 if battery.power_plugged else 0
    ser.write(f"{pct},{charging}\n".encode())
    time.sleep(10)
```

## Compiling

Open the project in PlatformIO and build/upload as usual.
