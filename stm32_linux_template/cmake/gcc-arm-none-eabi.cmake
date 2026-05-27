# 设置开发平台
set(CMAKE_SYSTEM_NAME Generic)

# 设置芯片架构
set(CMAKE_SYSTEM_PROCESSOR arm)


# 设置编译器类型
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)


# 设置工具链
set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)


# 设置语言编辑后生成的可执行文件的后缀
set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")


# 关闭CMake测试
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


set(TARGET_FLAGS "-mcpu=cortex-m3")


# 设置编译器编译时要加的参数
# 告知CPU类型 指令集 FPU ABI
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")

# ${CMAKE_C_FLAGS} 汇编继承 C 的 CPU/FPU 参数
# -x assembler-with-cpp 汇编经过cpp预处理器=汇编支持 #include/#ifdef
# -MMD 生成依赖文件 用于 增量编译
# -MP 防止某种依赖错误
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")

# ${CMAKE_C_FLAG} 追加编译参数
# -Wall 开启常见警告
# -fdata-sections 每个变量独立 section
# -ffunction-sections 每个函数独立 section
# 上面变量 函数独立 section 是为了配合 -Wl,--gc-sections 自动删除没用到的函数和变量 “链接器垃圾回收(GC)”
# -fstack-usage 生成栈分析信息
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fdata-sections -ffunction-sections -fstack-usage")

# ${CMAKE_C_FLAGS} c++继承c的编译参数
# -fno-rtti 关闭RTTI 运行时类型识别
# -fno-exceptions 关闭c++异常机制 c++的异常机制非常“重”
# -fno-threadsafe-statics 关闭局部 static 线程安全
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")


# -o0 不优化
# -g3 生成完整调试信息 给GDB OpenOCD Cortex-Debug使用
# -0s 优化代码体积 GCC优化等级：-o0 不优化 -o1 轻度优化 -o2 常规优化 -o3 激进优化 -os 优先减小代码体积
# -g0 不生成调试信息
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")


# 设置链接器参数
set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS}")

# ${CMAKE_EXE_LINKER_FLAGS} 追加参数
# -T \"${CMAKE_SOURCE_DIR}/STM32F103XB_FLASH.ld\ 指定 linker_script
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${CMAKE_SOURCE_DIR}/STM32F103XB_FLASH.ld\"")

# --specs=nano.specs 使用 nano libc (即C精简库)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")

# -Wl, 把后面的参数转发给linker(ld)
# -Wl,--gc-sections 删除无用代码 配合上面的 -ffunction-sections -fdata-sections 实现 HAL没用到的函数不进入最终ELF
# -Wl,-Map=${CMAKE_PROJECT_NAME}.map 生成映射文件(firmware.map) 这个文件记录：每个函数地址 Flash占用 RAM占用
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")

# --print-memory-usage 链接完成后打印MCU内存占用
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")

# 链接时需要额外链接哪些库
# m 链接数字库
set(TOOLCHAIN_LINK_LIBRARIES "m")


# 编译      main.c -> main.o 处理：语法 生成机器码
# 链接link  main.0 + gpio.o + uart.0 + led.o + ... -> firmware.elf 负责：拼接程序 分配内存 解析函数引用 删除无用代码 生成map文件 统计内存。。。