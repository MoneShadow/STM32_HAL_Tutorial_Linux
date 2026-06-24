# 再次设置CMake支持的最低版本
cmake_minimum_required(VERSION 3.22)

# 再次显式声明支持的语言
enable_language(C ASM)

# 设置macro变量(预处理宏)
# $<$<CONFIG:Debug>:DEBUG> Generator Expression 仅在 Debug 配置下自动添加DEBUG宏
set(MX_Defines_Syms 
	USE_HAL_DRIVER
	STM32F103xB
    $<$<CONFIG:Debug>:DEBUG>
)

# 设置头文件搜索路径变量
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Hw/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Sys/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Device/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/portable/GCC/ARM_CM3
    # 这里按照自己后面设置项目结构进行修改
)

# 设置应用层源文件 + MCU启动文件变量
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../startup_stm32f103xb.s 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Hw/Src/stm32f1xx_it.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Hw/Src/LED.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/User/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Sys/Src/rcc.c
    # 这里是User目录下的Core目录下的.c文件
)

# 设置HAL库源文件/CMSIS源文件变量
set(STM32_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Device/Source/system_stm32f1xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c

    # ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_
    # 这里是库里的.c文件
)

# 设置链接外部库变量
set(MX_LINK_DIRS
    # 供以后添加第三方库用的
)

# 设置 FreeRTOS 源文件
set(FreeRTOS_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/source/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/source/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/source/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/source/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/source/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/source/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/source/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/FreeRTOS/portable/GCC/ARM_CM3/port.c
)

# 设置最终ELF需要链接的库变量
set(MX_LINK_LIBS 
    STM32_Drivers
    ${TOOLCHAIN_LINK_LIBRARIES}
    
)

# 添加INTERFACE库
add_library(stm32_conf INTERFACE)
# 给该库添加用于传播的 include路径、宏定义、编译参数等
target_include_directories(stm32_conf INTERFACE ${MX_Include_Dirs})
# 给依赖该库的target自动添加宏定义
target_compile_definitions(stm32_conf INTERFACE ${MX_Defines_Syms})

# 添加OBJECT库
add_library(STM32_Drivers OBJECT)
# 给该库添加HAL库源文件 供该库编译生成.o文件
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
# 给该库添加编译时所需依赖(主要用的是HAL的头文件搜索路径 和 macro) 以及传播使用
target_link_libraries(STM32_Drivers PUBLIC stm32_conf)

# 给项目(即最终的ELF文件)添加应用层源文件
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MX_Application_Src})

# 给项目添加 FreeRTOS 源文件
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${FreeRTOS_Src})

# 添加外部库搜索路径 供链接阶段查找 .a/.so 文件
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MX_LINK_DIRS})

# 给最终ELF链接所需库 OBJECT库会把其生成的 .o 文件直接加入最终链接
target_link_libraries(${CMAKE_PROJECT_NAME} ${MX_LINK_LIBS})

# ADDITIONAL_CLEAN_FILES 额外需要clean的文件
# 告诉CMake clean的时候把.map映射文件也删掉 
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# 检查语言标准
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()