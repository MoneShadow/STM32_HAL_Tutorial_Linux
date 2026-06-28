# === GDB 启动时自动执行的命令 ===
# 连接 OpenOCD
target extended-remote localhost:3333
# 复位芯片
monitor reset halt
# 烧写固件
load
# 设 main 断点
break main
# 运行到 main
continue
set breakpoint auto-hw on
set print pretty on
set pagination off
set history save on
echo OK