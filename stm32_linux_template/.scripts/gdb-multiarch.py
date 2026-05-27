import subprocess

result = subprocess.run([
    "gdb-multiarch",
    "build/Debug/stm32_linux_template.elf"
])

if result == 0:
    print("Sucesess")
else:
    print("Failed")