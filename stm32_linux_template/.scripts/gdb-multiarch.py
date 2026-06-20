import subprocess
from pathlib import Path

project_name = Path(__file__).resolve().parent.parent.name
elf_path = f"build/Debug/{project_name}.elf"

result = subprocess.run([
    "gdb-multiarch",
    elf_path
])

if result.returncode == 0:
    print("Success")
else:
    print("Failed")
