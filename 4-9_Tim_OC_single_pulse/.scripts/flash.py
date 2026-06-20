import subprocess
from pathlib import Path

project_name = Path(__file__).resolve().parent.parent.name
elf_path = f"build/Debug/{project_name}.elf"

result = subprocess.run([
    "openocd",
    "-f", "interface/stlink.cfg",
    "-f", "target/stm32f1x.cfg",
    "-c",
    f"program {elf_path} verify reset exit"
])

if result.returncode == 0:
    print(f"Flash success: {elf_path}")
else:
    print(f"Flash failed (exit code {result.returncode})")
