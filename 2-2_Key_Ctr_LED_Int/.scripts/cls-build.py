import subprocess

result = subprocess.run([
    "find",
    "build/Debug",
    "-mindepth",
    "1",
    "-delete"
])

if result.returncode == 0:
    print("Success")
else:
    print("Failed")