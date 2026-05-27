import subprocess

result = subprocess.run([
    "gdb-multiarch",
    "build/Debug/1-1_InitSystemClock.elf"
])

if result == 0:
    print("Sucesess")
else:
    print("Failed")