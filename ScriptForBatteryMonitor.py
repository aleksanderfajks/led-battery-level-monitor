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
