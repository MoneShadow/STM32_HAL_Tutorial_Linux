# 设置编译器最低支持版本
cmake_minimum_required(VERSION 3.22)

# 设置C语言标准
set(CMAKE_C_STANDARD 11)
# 强制语言标准不许降级
set(CMAKE_C_STANDARD_REQUIRED ON)   
# 开启语言扩展功能
set(CMAKE_C_EXTENSIONS ON)          

# 检查配置生成以及构建的类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# 设置项目名称
set(CMAKE_PROJECT_NAME 4-3_Tim_DMA)

# 开启生成 compile_command 供语法检查用
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# 设置项目(空项目)
project(${CMAKE_PROJECT_NAME})
# 打印构建类型
message("Build type: " ${CMAKE_BUILD_TYPE})

# 显式声明支持的语言(C ASM)
enable_language(C ASM)

# 添加可执行文件(空可执行文件 现在先不添加源码)
add_executable(${CMAKE_PROJECT_NAME})

# 给项目添加子目录(该目录下的CML负责includepath macro 源文件等东西)
add_subdirectory(cmake/stm32)

# 用于连接外部库(在这里HAL不被认为是外部库)
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# 添加源码文件
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# 添加头文件搜索路径
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# 添加macro
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# 移出这个特定的异常依赖
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# 给目标连接库(这里链接的stm32_conf是一个INTERFAE库)
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32_conf

    # Add user defined libraries
)

add_custom_target(flash
    COMMAND openocd
    -f interface/stlink.cfg
    -f target/stm32f1x.cfg
    -c "program $<TARGET_FILE:${CMAKE_PROJECT_NAME}> verify reset exit"
    DEPENDS ${CMAKE_PROJECT_NAME}
)