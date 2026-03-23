# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)  

include(pico_sdk_import.cmake)        # ← FALTAVA

project(jogo-reflexo C CXX ASM)      # ← FALTAVA

pico_sdk_init()                       # ← FALTAVA

set(PICO_BOARD pico CACHE STRING "Board type")

add_executable(jogo-reflexo
    main.c
)

target_link_libraries(jogo-reflexo
    pico_stdlib
    hardware_gpio
    hardware_timer
)

pico_add_extra_outputs(jogo-reflexo)