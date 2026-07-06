import subprocess

result = subprocess.run([
    "cmake",
    "--build",
    "build/Debug"
])

if result.returncode == 0:
    print("Success")
else:
    print("Failed")