import subprocess
import time

openocd = subprocess.Popen([
    "openocd",
    "-f", "interface/stlink.cfg",
    "-f", "target/stm32f1x.cfg"
])

print("Openocd Was Opened")

try:
    while True:
        time.sleep(1)

except KeyboardInterrupt:
    print("Openocd Is Offing...")
    openocd.kill()