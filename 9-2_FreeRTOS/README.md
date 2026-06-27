# STM32F103 裸机模板移植 FreeRTOS 全程记录

> **硬件**: STM32F103XB（中密度，128KB Flash）  
> **FreeRTOS 版本**: V11.3.0  
> **编译器**: arm-none-eabi-gcc (CMake 构建)  
> **移植起点**: 裸机模板 — LED + RCC 时钟配置 + HAL 库  
> **移植终点**: FreeRTOS 双任务点灯程序  

---

## 目录

1. [第一步：复制裸机模板](#第一步复制裸机模板)
2. [第二步：引入 FreeRTOS 内核源码](#第二步引入-freertos-内核源码)
3. [第三步：CMake 构建系统接入](#第三步cmake-构建系统接入)
4. [第四步：修改 FreeRTOSConfig.h 适配硬件](#第四步修改-freertosconfigh-适配硬件)
5. [第五步：安装中断服务函数](#第五步安装中断服务函数)
6. [第六步：编写示例小程序](#第六步编写示例小程序)
7. [附录：关键概念](#附录关键概念)

---

## 第一步：复制裸机模板

**Commit**: `2616e69` — *1. 复制stm32模板程序*

以已有的裸机模板项目为起点，复制到 `FreeRTOS_template/` 目录。

### 模板基线内容

```
FreeRTOS_template/
├── Core/
│   ├── Hw/Inc/          LED.h, stm32f1xx_it.h
│   ├── Hw/Src/          LED.c, stm32f1xx_it.c
│   ├── Sys/Inc/         rcc.h
│   ├── Sys/Src/         rcc.c
│   └── User/            main.c (空壳)
├── Drivers/
│   ├── CMSIS/           Device + Include
│   └── STM32F1xx_HAL_Driver/
├── cmake/
│   ├── gcc-arm-none-eabi.cmake
│   └── stm32/CMakeLists.txt
├── CMakeLists.txt
├── CMakePresets.json
├── startup_stm32f103xb.s
├── STM32F103XB_FLASH.ld
└── .vscode/             launch.json, tasks.json
```

裸机模板已经具备：

- CMSIS 设备头文件和启动文件
- STM32F1xx HAL 驱动库
- LED 驱动（PA1, PA2）
- RCC 时钟配置（72MHz, HSE + PLL）
- CMake 构建系统 + OpenOCD flash 烧录

此时 `main.c` 是空的——这是我们移植的起点。

---

## 第二步：引入 FreeRTOS 内核源码

**Commit**: `f1a2568` — *2.必要移植项*

### 新增内容

#### 目录结构

将 FreeRTOS V11.3.0 内核放入项目根目录 `FreeRTOS/`，与 `Drivers/` 同级：

```
FreeRTOS/
├── include/           22 个头文件 + CMakeLists.txt
│   ├── FreeRTOS.h     task.h     queue.h     semphr.h
│   ├── timers.h       event_groups.h         stream_buffer.h
│   ├── message_buffer.h  list.h  croutine.h  atomic.h
│   └── ...
├── source/            7 个内核源文件 + CMakeLists.txt
│   ├── tasks.c        queue.c     list.c      timers.c
│   ├── event_groups.c stream_buffer.c         croutine.c
│   └── ...
└── portable/
    ├── ARM_CM3/       port.c + portmacro.h         ← STM32F103 是 Cortex-M3
    └── MemMang/       heap_4.c                     ← 动态内存管理
```

#### FreeRTOSConfig.h

放在 `Core/Sys/Inc/FreeRTOSConfig.h`，这是给 FreeRTOS 内核看的配置文件，定义了硬件参数和功能开关。

> ⚠️ 此阶段 `FreeRTOSConfig.h` 还是模板默认值——20MHz 时钟、100Hz tick、64-bit tick 类型等，后续第四步才调整为 STM32F103 的实际参数。

### 为什么把 FreeRTOS 放根目录而不是 Drivers/ 下？

因为 FreeRTOS 不是"驱动"，它是操作系统内核。与 CMSIS、HAL 平级放置更清晰地表达了这种层次关系：

```
FreeRTOS/     ← OS 内核
Drivers/
  ├── CMSIS/  ← ARM 抽象层
  └── HAL/    ← 外设驱动
```

---

## 第三步：CMake 构建系统接入

**Commit**: `1328cd6` — *3.给新添加的项目接入CMake管理*

仅修改 `cmake/stm32/CMakeLists.txt`（+18 行），三处改动：

### 3.1 添加头文件搜索路径

```cmake
set(MX_Include_Dirs
    # ... 原有路径 ...
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS/portable/ARM_CM3  # portmacro.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeRTOS/include           # FreeRTOS.h 等
)
```

两个路径缺一不可：

- `FreeRTOS/include/` — 内核 API 头文件（task.h, queue.h...）
- `FreeRTOS/portable/ARM_CM3/` — 平台移植宏（portmacro.h, 含临界区、优先级转换等）

### 3.2 定义 FreeRTOS 源文件列表

```cmake
set(FreeRTOS_Src
    .../FreeRTOS/source/croutine.c
    .../FreeRTOS/source/event_groups.c
    .../FreeRTOS/source/list.c
    .../FreeRTOS/source/queue.c
    .../FreeRTOS/source/stream_buffer.c
    .../FreeRTOS/source/tasks.c
    .../FreeRTOS/source/timers.c
    .../FreeRTOS/portable/MemMang/heap_4.c        # 内存管理
    .../FreeRTOS/portable/ARM_CM3/port.c          # Cortex-M3 移植层
)
```

9 个文件：7 内核 + 1 内存 + 1 移植。

### 3.3 加入最终链接

```cmake
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${FreeRTOS_Src})
```

FreeRTOS 源文件通过 `target_sources()` 直接加入可执行文件，与 main.c、LED.c 等应用文件一起编译链接。

> ⚠️ CMake 不会自动扫描目录，任何新建 `.c` 文件都必须手动加入源文件列表。

---

## 第四步：修改 FreeRTOSConfig.h 适配硬件

**Commit**: `ffb0fb4` — *4. 修改配置文件*

模板默认值是为 QEMU 模拟器准备的，必须根据 STM32F103 实际硬件调整。

### 修改清单

| 配置项                                 | 旧值       | 新值              | 说明                                |
| -------------------------------------- | ---------- | ----------------- | ----------------------------------- |
| `configCPU_CLOCK_HZ`                   | `20000000` | `72000000`        | STM32F103 系统时钟 72MHz            |
| `configTICK_RATE_HZ`                   | `100`      | `1000`            | 见[第五步](#第五步安装中断服务函数) |
| `configUSE_TIME_SLICING`               | `0`        | `1`               | 开启同优先级时间片轮转              |
| `configTICK_TYPE_WIDTH_IN_BITS`        | `64_BITS`  | `32_BITS`         | Cortex-M3 是 32 位核                |
| `configKERNEL_INTERRUPT_PRIORITY`      | `0`        | `(15 << 4)` = 240 | 内核中断用最低优先级                |
| `configMAX_SYSCALL_INTERRUPT_PRIORITY` | `0`        | `(5 << 4)` = 80   | 仅优先级 ≥5 的 ISR 可调 API         |
| `configCHECK_FOR_STACK_OVERFLOW`       | `2`        | `0`               | 开发阶段关闭，后续可按需打开        |

### 中断优先级设计

Cortex-M3 NVIC 用 4 位来表示优先级（0~15），FreeRTOS 要求在此基础上左移 4 位：

```
寄存器: [7:4] 优先级  [3:0] 子优先级（未使用）

(priority << 4) → 填入 bit[7:4]
```

最终效果：

```
优先级  0 ─── 最高，不可调用 FreeRTOS API
优先级  1      （供高实时性中断使用）
优先级  2
优先级  3
优先级  4
  ─────── configMAX_SYSCALL_INTERRUPT_PRIORITY = 80 ───────
优先级  5 ─── 可以调用 xQueueSendFromISR() 等
优先级  6
  ...
优先级 14
优先级 15 ─── 最低，SysTick / PendSV 在此
  ─────── configKERNEL_INTERRUPT_PRIORITY = 240 ──────────
```

**为什么优先级 0~4 不能调 FreeRTOS API？**  
因为 FreeRTOS 临界区通过屏蔽"优先级 ≤ configMAX_SYSCALL"的中断来保护数据一致性。如果允许优先级 0~4 调 API，这些高优先级 ISR 在临界区内仍可运行，可能破坏内核数据结构。

---

## 第五步：安装中断服务函数

### 方案选择：共用 SysTick

**Commit**: `161dfa4` → `93ed69a`

#### 背景

FreeRTOS 需要接管三个 Cortex-M3 异常：

- **SVC** (Supervisor Call) — 启动第一个任务
- **PendSV** (Pendable Service Call) — 上下文切换
- **SysTick** — 系统心跳

三种 ISR 有两种路由方式：

| 方式                 | 做法                              | SVC/PendSV | SysTick |
| -------------------- | --------------------------------- | :--------: | :-----: |
| **Direct Routing**   | FreeRTOS 函数直接用宏映射到向量表 |     ✅      |    ❌    |
| **Indirect Routing** | 应用层 handler 转发给 FreeRTOS    |     —      |    ✅    |

对于 SVC 和 PendSV，用 Direct Routing 是最干净的——FreeRTOS 的 `port.c` 已提供完整实现，应用层不该碰。

对于 SysTick，情况复杂一些。因为 **HAL 也需要 SysTick** 来做超时检测（`HAL_GetTick()` 被 I2C/SPI/UART 等大量 HAL 驱动内部使用）。

> 曾考虑换用 TIM6 做 HAL 时间基准，让 SysTick 归 FreeRTOS 独占。但 STM32F103XB（中密度）没有 TIM6/TIM7——那是 F103xC/D/E 高密度型号才有的基本定时器。所以选择**共用 SysTick**。

#### 最终方案

```
SysTick IRQ (1kHz)
  └─ SysTick_Handler()        ← stm32f1xx_it.c 实现
       ├─ HAL_IncTick()        ← 喂 HAL 时间基准
       └─ xPortSysTickHandler() ← 喂 FreeRTOS tick
```

#### FreeRTOSConfig.h 改动

```c
// SVC/PendSV: Direct Routing — FreeRTOS port.c 直接接管
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
// SysTick: Indirect Routing — stm32f1xx_it.c 桥接 HAL + FreeRTOS
// (不再映射 xPortSysTickHandler)

#define configTICK_RATE_HZ   1000  // 与 HAL 默认 SysTick 1kHz 一致
```

#### stm32f1xx_it.c 改动

```c
#include "FreeRTOS.h"
#include "task.h"
extern void xPortSysTickHandler(void);

void SysTick_Handler(void) {
    HAL_IncTick();       // HAL 时间基准
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();  // FreeRTOS tick
    }
}

// SVC_Handler 和 PendSV_Handler 注释掉，交给 FreeRTOS port.c
```

**`xTaskGetSchedulerState()` 判断为什么有必要？**  
`main()` 中在启动调度器前会调用 `HAL_Init()` → `HAL_InitTick()` → 配置 SysTick 并立即开始中断。如果 ISR 中直接调用 `xPortSysTickHandler()`，此时 FreeRTOS 内核尚未初始化，可能触发 assert 或 HardFault。

---

## 第六步：编写示例小程序

**Commit**: `2dc72b7` + `58723cb` — *6. 添加一个示例小程序* + *6.1 补充遗漏的项目管理*

### 新建文件

**FreeRTOS_Demo.h**

```c
#ifndef __FREERTOS_DEMO_H
#define __FREERTOS_DEMO_H
void FreeRTOS_Start(void);
#endif
```

**FreeRTOS_Demo.c**

```c
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"

static void task1(void *arg);
static void task2(void *arg);

TaskHandle_t task1_handler;
TaskHandle_t task2_handler;

void FreeRTOS_Start(void) {
    xTaskCreate(task1, "task1", 128, NULL, 4, &task1_handler);
    xTaskCreate(task2, "task2", 128, NULL, 4, &task2_handler);
    vTaskStartScheduler();      // 永不返回
}

static void task1(void *arg) {
    while (1) {
        LED1_Toggle();                      // PA1 翻转
        vTaskDelay(pdMS_TO_TICKS(500));     // 延时 500ms
    }
}

static void task2(void *arg) {
    while (1) {
        LED2_Toggle();                      // PA2 翻转
        vTaskDelay(pdMS_TO_TICKS(1000));    // 延时 1000ms
    }
}
```

| 参数                 | 值        | 含义                                                   |
| -------------------- | --------- | ------------------------------------------------------ |
| 栈大小               | 128       | 128 words = 512 bytes（`configMINIMAL_STACK_SIZE`）    |
| 优先级               | 4         | `configMAX_PRIORITIES = 5`，即 0~4，两个 task 同优先级 |
| `pdMS_TO_TICKS(500)` | 500 ticks | `configTICK_RATE_HZ=1000` 时 1 tick = 1ms              |

### 修改 LED 驱动

```c
// LED.h 新增
void LED1_Toggle(void);   // PA1
void LED2_Toggle(void);   // PA2

// LED.c 新增
void LED1_Toggle(void) { HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1); }
void LED2_Toggle(void) { HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2); }
```

### 修改 main.c

```c
#include "LED.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rcc.h"
#include "FreeRTOS_Demo.h"

int main(void) {
    HAL_Init();          // ① 初始化 HAL 库 + 配置 SysTick 1ms
    RCC_InitClock();     // ② 系统时钟 72MHz (HSE + PLL)
    LED_Init();          // ③ 初始化 GPIO PA1, PA2
    FreeRTOS_Start();    // ④ 创建任务 + 启动调度器（永不返回）
    while (1) {}         // ⑤ 理论上不会执行到这里
}
```

**执行流程**：

```
HAL_Init()
  └─ HAL_InitTick()  → 配置 SysTick 1kHz
                        SysTick 中断开始触发
                        (此时仅 HAL_IncTick 生效，FreeRTOS 尚未启动)

RCC_InitClock()      → 72MHz

LED_Init()           → GPIO 初始化

FreeRTOS_Start()
  ├─ xTaskCreate(task1)  → 创建 task1, 512 字节栈, 优先级 4
  ├─ xTaskCreate(task2)  → 创建 task2, 512 字节栈, 优先级 4
  └─ vTaskStartScheduler()
       ├─ 初始化内核列表
       ├─ vPortSVCHandler → 启动第一个任务
       └─ 调度器接管 → task1/task2 交替运行
            SysTick 中断同时喂 HAL + FreeRTOS
```

### 补充 CMake（6.1）

`cmake/stm32/CMakeLists.txt` 中补上 `FreeRTOS_Demo.c`：

```cmake
set(MX_Application_Src
    # ...
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Hw/Src/FreeRTOS_Demo.c  # ← 新增
)
```

忘记加的后果：链接时报 `undefined reference to FreeRTOS_Start`。

---

## 移植完成！最终项目结构

```
FreeRTOS_template/
├── Core/
│   ├── Hw/Inc/
│   │   ├── LED.h                    # LED_Init + LED1/2_Toggle
│   │   ├── FreeRTOS_Demo.h          # ⭐ FreeRTOS_Start()
│   │   └── stm32f1xx_it.h
│   ├── Hw/Src/
│   │   ├── LED.c
│   │   ├── FreeRTOS_Demo.c          # ⭐ 两个 LED 闪烁任务
│   │   └── stm32f1xx_it.c           # ⭐ SysTick 桥接 HAL + FreeRTOS
│   ├── Sys/Inc/
│   │   ├── rcc.h
│   │   └── FreeRTOSConfig.h         # ⭐ 内核配置
│   ├── Sys/Src/
│   │   └── rcc.c
│   └── User/
│       └── main.c                   # ⭐ 完整初始化链
├── FreeRTOS/                        # ⭐ FreeRTOS V11.3.0
│   ├── include/                     # 头文件
│   ├── source/                      # 内核源码
│   └── portable/
│       ├── ARM_CM3/                 # Cortex-M3 移植
│       └── MemMang/                 # heap_4.c
├── Drivers/
│   ├── CMSIS/
│   └── STM32F1xx_HAL_Driver/
├── cmake/
│   └── stm32/CMakeLists.txt         # ⭐ 含 FreeRTOS 头文件 + 源文件
├── CMakeLists.txt
├── startup_stm32f103xb.s
└── STM32F103XB_FLASH.ld
```

（⭐ = 本次移植新增或修改的文件）

---

## 附录：关键概念

### A. Direct Routing vs Indirect Routing

```
Direct Routing:
  #define vPortSVCHandler SVC_Handler
  → port.c 中的 vPortSVCHandler 直接变为 SVC_Handler
  → ISR 入口 = FreeRTOS 函数，零跳转开销
  → 应用层不能定义同名函数，否则链接冲突

Indirect Routing:
  stm32f1xx_it.c 定义 SysTick_Handler
  → ISR 先到应用层
  → 应用层调 xPortSysTickHandler() 转发给 FreeRTOS
  → 额外一次函数调用开销
  → 可以在转发前/后插入自己的逻辑（比如 HAL_IncTick）
```

### B. 为什么 configCPU_CLOCK_HZ 很重要

FreeRTOS 用 `configCPU_CLOCK_HZ / configTICK_RATE_HZ` 计算 SysTick 重装值。如果 `configCPU_CLOCK_HZ` 和实际时钟不匹配，所有延时 API 的时间就会偏离，比如 `vTaskDelay(pdMS_TO_TICKS(1000))` 实际可能只延时了 278ms（如果实际是 20MHz 而配置是 72MHz）。

### C. heap_4.c 为什么是默认选择

| 方案       | 特点                                     |
| ---------- | ---------------------------------------- |
| heap_1     | 只能分配不能释放，简单                   |
| heap_2     | 能分配能释放，但不合并碎片（已废弃）     |
| **heap_4** | 最佳匹配 + 相邻空闲块合并，通用性最好    |
| heap_5     | 同 heap_4，但支持跨多个不连续的 RAM 区域 |

### D. configTICK_RATE_HZ 快慢权衡

| 频率       | tick 间隔 | 优点                        | 缺点                    |
| ---------- | --------- | --------------------------- | ----------------------- |
| 100Hz      | 10ms      | ISR 开销低                  | 最小延时分辨率 10ms     |
| **1000Hz** | 1ms       | 毫秒级延时精度，与 HAL 对齐 | tick ISR 每秒 1000 次   |
| 10kHz      | 100μs     | 高精度定时                  | 显著 CPU 开销，极少使用 |

### E. 移植检查清单

当你把 FreeRTOS 移植到新 STM32 项目时，问自己：

- [ ] FreeRTOS 源码放进来了吗？（`FreeRTOS/` 目录）
- [ ] CMake 能找到头文件吗？（`MX_Include_Dirs`）
- [ ] CMake 能编译所有 `.c` 吗？（`FreeRTOS_Src` + `target_sources`）
- [ ] `configCPU_CLOCK_HZ` 和实际系统时钟一致吗？
- [ ] `configTICK_RATE_HZ` 和 SysTick 中断频率一致吗？（共用时为 1000）
- [ ] 中断优先级适配 Cortex-M3 了吗？（`priority << 4`）
- [ ] `SVC_Handler` 和 `PendSV_Handler` 没在 stm32f1xx_it.c 中定义吗？
- [ ] `SysTick_Handler` 中同时调用了 `HAL_IncTick()` 和 `xPortSysTickHandler()` 吗？
- [ ] `HAL_Init()` 在 `vTaskStartScheduler()` 之前调用了吗？
- [ ] 每个新建 `.c` 文件都加到 CMake 源文件列表了吗？