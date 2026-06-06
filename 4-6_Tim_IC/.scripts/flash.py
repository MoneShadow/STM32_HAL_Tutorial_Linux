import subprocess

result = subprocess.run([
    "openocd",
    "-f", "interface/stlink.cfg",
    "-f", "target/stm32f1x.cfg",
    "-c",
    "program build/Debug/4-6_Tim_IC.elf verify reset exit"
])

if result.returncode == 0:
    print("Success")
else:
    print("Failed")