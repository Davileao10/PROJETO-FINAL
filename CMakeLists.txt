# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)
set(PICO_BOARD pico CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(alarm_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(alarm_project
    main.c
    pwm_irq.c
    inc/ssd1306.c
)

target_include_directories(alarm_project PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${CMAKE_CURRENT_LIST_DIR}/generated  # Adicionado para ws2812.pio.h manual
)

target_link_libraries(alarm_project
    pico_stdlib
    hardware_pwm
    hardware_i2c
    hardware_gpio
    hardware_irq
    hardware_pio
)

pico_add_extra_outputs(alarm_project)
pico_enable_stdio_usb(alarm_project 1)
pico_enable_stdio_uart(alarm_project 1)