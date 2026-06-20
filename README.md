# STM32 HAL 外设实战：基于 GNU/Linux 工具链

> **这不是一个"空白项目"——这是一份 STM32F103 全外设的 GNU 工具链实战记录。**

## 这个仓库和大多数 STM32 教程的区别

|              | 大多数教程                     | 这个仓库                                             |
| ------------ | ------------------------------ | ---------------------------------------------------- |
| **开发环境** | Windows + Keil MDK（一键下载） | **Linux + VS Code + CMake + arm-gcc**                |
| **构建系统** | IDE 自动管理，不透明           | **CMake + Ninja**，完全可控                          |
| **烧录调试** | Keil 内置                      | **OpenOCD + ST-Link**，Python 脚本驱动               |
| **版本管理** | 通常没有                       | **155+ commits**，每个外设独立提交                   |
| **可复现性** | 依赖特定 IDE 版本              | 任何装了 arm-gcc + OpenOCD 的 Linux 机器都能构建     |
| **编辑体验** | Keil 编辑器                    | **VS Code + clangd**（语法高亮、自动补全、跳转定义） |

> **核心价值**：这个仓库证明了你**理解编译-链接-烧录的全流程**，而不是只会点 Keil 的下载按钮。当你换到 RISC-V、换到其他 ARM 芯片、换到 Zephyr RTOS——工具链思想完全复用。

## 已覆盖的外设（24 个示例）

### 1. 时钟系统

| 目录                                          | 内容                                 |
| --------------------------------------------- | ------------------------------------ |
| [`1-1_InitSystemClock`](1-1_InitSystemClock/) | 系统时钟初始化，HSE/PLL 配置到 72MHz |

### 2. GPIO 与中断

| 目录                                            | 内容                         |
| ----------------------------------------------- | ---------------------------- |
| [`2-1_Key_Ctr_LED_Poll`](2-1_Key_Ctr_LED_Poll/) | 按键控制 LED（轮询方式）     |
| [`2-2_Key_Ctr_LED_Int`](2-2_Key_Ctr_LED_Int/)   | 按键控制 LED（外部中断方式） |

### 3. UART 串口通信

| 目录                                          | 内容                |
| --------------------------------------------- | ------------------- |
| [`3-1_Uart_RTx_Poll`](3-1_Uart_RTx_Poll/)     | 串口收发（轮询）    |
| [`3-2_Uart_RTx_IT`](3-2_Uart_RTx_IT/)         | 串口收发（中断）    |
| [`3-3_Uart_RTx_DMA`](3-3_Uart_RTx_DMA/)       | 串口收发（DMA）     |
| [`3-4_Uart_RTx_Printf`](3-4_Uart_RTx_Printf/) | printf 重定向到串口 |

### 4. 定时器（最完整的系列，12 个示例）

| 目录                                                  | 内容                                 |
| ----------------------------------------------------- | ------------------------------------ |
| [`4-1_Tim_Poll`](4-1_Tim_Poll/)                       | 基本定时（轮询）                     |
| [`4-2_Tim_IT`](4-2_Tim_IT/)                           | 基本定时（中断）                     |
| [`4-3_Tim_DMA`](4-3_Tim_DMA/)                         | 基本定时（DMA）                      |
| [`4-4_Tim_OutMode`](4-4_Tim_OutMode/)                 | 定时器输出模式                       |
| [`4-5_Tim_OutMode_IT_DMA`](4-5_Tim_OutMode_IT_DMA/)   | 输出模式 + 中断/DMA                  |
| [`4-6_Tim_IC`](4-6_Tim_IC/)                           | 输入捕获（测量脉宽/频率）            |
| [`4-7_Tim_OC`](4-7_Tim_OC/)                           | 输出比较                             |
| [`4-8_Tim_OC_PWM`](4-8_Tim_OC_PWM/)                   | PWM 输出（电机/舵机控制基础）        |
| [`4-9_Tim_OC_single_pulse`](4-9_Tim_OC_single_pulse/) | 单脉冲模式                           |
| [`4-10_Tim_OC_Encoder`](4-10_Tim_OC_Encoder/)         | 编码器模式（电机测速）               |
| [`4-11_Tim_IC_ETR`](4-11_Tim_IC_ETR/)                 | 外部触发输入                         |
| [`4-12_Tim_IC_PWM`](4-12_Tim_IC_PWM/)                 | PWM 输入捕获（同时测量频率和占空比） |

### 5. 模拟与通信外设

| 目录                  | 内容                             |
| --------------------- | -------------------------------- |
| [`5-1_ADC`](5-1_ADC/) | ADC 模数转换（单通道/多通道）    |
| [`6-1_I2C`](6-1_I2C/) | I2C 通信（读写 EEPROM / 传感器） |
| [`7-1_SPI`](7-1_SPI/) | SPI 通信（读写 Flash / 显示屏）  |
| [`8-1_RTC`](8-1_RTC/) | 实时时钟（后备电池域）           |

## 工具链

```
编辑器:    VS Code + clangd（代码补全/跳转/诊断）
编译器:    arm-none-eabi-gcc（GNU Arm Embedded Toolchain）
构建:      CMake 3.22+ + Ninja
调试:      OpenOCD + ST-Link V2 + arm-none-eabi-gdb
烧录:      Python 脚本驱动 OpenOCD（一键 Flash）
```

## 快速开始

### 前置条件

```bash
# Ubuntu/Debian
sudo apt install cmake ninja-build gcc-arm-none-eabi openocd python3

# Arch
sudo pacman -S cmake ninja arm-none-eabi-gcc openocd python3
```

### 构建 & 烧录

```bash
# 进入任意示例目录
cd 4-8_Tim_OC_PWM

# 配置（Debug 模式）
cmake --preset Debug

# 构建
cmake --build build/Debug -j$(nproc)

# 烧录（需要连接 ST-Link）
cmake --build build/Debug --target flash
```

或者使用 VS Code 快捷键：`Ctrl+Shift+B` → 选择 Build / Flash / Build & Flash。

### 从模板创建新项目

```bash
cp -r stm32_linux_template my_new_project
cd my_new_project
# 修改 CMakeLists.txt 中的项目名
# 在 Core/Src/ 和 Core/Inc/ 下添加你的代码
```

## 目标芯片

- **MCU**: STM32F103C8T6（Blue Pill / 最小系统板）
- **调试器**: ST-Link V2
- **移植到其他 STM32F1 芯片**：修改 `CMakeLists.txt` 中的 `STM32F103XB_FLASH.ld` 链接脚本和启动文件即可

## 仓库结构

```
每个示例目录/
├── .vscode/           # VS Code tasks（Build / Flash / Debug）
├── .scripts/           # Python 构建和烧录脚本
├── cmake/              # 工具链文件（gcc-arm-none-eabi.cmake）+ STM32 CMake 模块
├── Core/
│   ├── Inc/            # 用户头文件（main.h 等）
│   └── Src/            # 用户源码（main.c + 外设初始化）
├── Drivers/            # STM32 HAL 库（CMSIS + HAL）
├── CMakeLists.txt      # 顶层构建配置
├── CMakePresets.json   # Debug / Release 预设
└── startup_stm32f103xb.s  # 启动代码（GNU 汇编风格）
```

## 为什么用 GNU 工具链而不是 Keil？

1. **免费开源**——没有授权费，在任何机器上都能搭建
2. **理解底层**——从编译、链接到烧录，每一步你都看得见。面试时被问到"程序是怎么从 C 代码变成 MCU 上运行的二进制"，你能讲清楚
3. **跨架构复用**——这套 CMake + GCC 工作流切换到 RISC-V、ESP32、Zephyr RTOS 几乎零成本
4. **CI/CD 就绪**——命令行构建天然适合自动化测试和持续集成
5. **简历信号**——"会用 GNU 工具链独立搭建 STM32 开发环境"比"会用 Keil"有说服力得多

## 后续学习方向

这个仓库覆盖了 STM32 裸机外设。下一步的计划：

- [ ] **FreeRTOS 多任务项目**——在已有外设基础上引入实时操作系统
- [ ] **RISC-V 跨架构验证**——用同一套 CMake + GCC 工具链在 ESP32-C3 上跑通
- [ ] **完整系统项目**——多传感器数据采集 + 显示 + 通信

## License

MIT

---

*如果你也在用 GNU 工具链学习 STM32，欢迎 Star / Fork。有问题可以提 Issue。*

