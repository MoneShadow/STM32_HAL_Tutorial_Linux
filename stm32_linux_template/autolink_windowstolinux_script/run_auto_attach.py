"""
ST-Link 自动监听 & 连接 WSL 工具
双击此文件即可运行，自动申请管理员权限。
"""

import ctypes
import subprocess
import sys
import os


def is_admin():
    """检查是否以管理员权限运行"""
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except Exception:
        return False


def run_as_admin():
    """以管理员权限重新启动当前脚本（会弹出 UAC 确认框）"""
    script = os.path.abspath(__file__)
    ctypes.windll.shell32.ShellExecuteW(
        None,               # hwnd
        "runas",            # 请求管理员权限
        sys.executable,     # python.exe 路径
        f'"{script}"',      # 要执行的脚本
        None,               # 工作目录（None = 当前目录）
        1,                  # SW_SHOWNORMAL
    )


def main():
    # 切换到脚本所在目录
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)

    ps1_file = os.path.join(script_dir, "auto_attach.ps1")

    if not os.path.exists(ps1_file):
        print(f"错误: 找不到 {ps1_file}")
        input("按回车键退出...")
        sys.exit(1)

    print("正在启动 ST-Link 自动监听...")
    print("关闭此窗口即可停止监听。\n")

    # 启动 PowerShell 脚本
    subprocess.call([
        "powershell",
        "-ExecutionPolicy", "Bypass",
        "-File", ps1_file,
    ])

    print("\n监听已停止。")
    input("按回车键退出...")


if __name__ == "__main__":
    if is_admin():
        # 已经是管理员，直接运行
        main()
    else:
        # 不是管理员，重新以管理员身份启动（会弹出 UAC 窗口）
        run_as_admin()
