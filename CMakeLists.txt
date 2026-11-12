# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

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
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(arcade-game C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(arcade-game
    src/main.cpp
    src/game.cpp
    src/engine/input/input_manager.cpp
    src/engine/input/ir_remote.cpp
    src/engine/output/screen.cpp
    src/engine/output/display_manager.cpp
    src/game/main_menu.cpp
    src/game/accuracy.cpp
    src/game/reaction.cpp
    src/game/memory.cpp
)

pico_set_program_name(arcade-game "arcade-game")
pico_set_program_version(arcade-game "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(arcade-game 0)
pico_enable_stdio_usb(arcade-game 0)

add_subdirectory(third-party/pico-st7735/lib-st7735)

# Add the standard library to the build
target_link_libraries(arcade-game
        pico_stdlib
        hardware_spi
        hardware_timer
        hardware_clocks
        st7735
        )

# Add the standard include files to the build
target_include_directories(arcade-game PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/third-party/pico-st7735/lib-st7735/include
        ./src
)

target_compile_definitions(arcade-game PRIVATE
    SPI_PORT=spi1
    SPI_TX=11
    SPI_SCK=10
    SPI_TFT_PORT=spi1
    SPI_TFT_CS=9
    SPI_TFT_RST=12
    SPI_TFT_DC=13
    TFT_ENABLE_BLACK
    TFT_ENABLE_RESET
TFT_ENABLE_TEXT
TFT_ENABLE_SHAPES
)

target_compile_definitions(st7735 PRIVATE
    SPI_PORT=spi1
    SPI_TX=11
    SPI_SCK=10
    SPI_TFT_PORT=spi1
    SPI_TFT_CS=9
    SPI_TFT_RST=12
    SPI_TFT_DC=13
    TFT_ENABLE_BLACK
    TFT_ENABLE_RESET
TFT_ENABLE_TEXT
TFT_ENABLE_SHAPES
)

pico_add_extra_outputs(arcade-game)

