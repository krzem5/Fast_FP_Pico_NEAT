cmake_minimum_required(VERSION 3.12)
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_INIT_CMAKE_FILE ${PICO_SDK_PATH}/pico_sdk_init.cmake)
include(${PICO_SDK_INIT_CMAKE_FILE})
project(neat LANGUAGES C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_compile_options(-Wall -Wno-format -Werror -O3 -g0 -fdata-sections -ffunction-sections -Wl,--gc-sections)
include_directories(src/include)
include_directories(src/include/usb/config)
add_executable(neat
	src/main.c
	src/neat/neat.S
	src/neat/neat_tables.S
	src/usb/descriptor.c
	src/usb/parse_data.c
	src/usb/protocol.c
	src/usb/serial.c
)
pico_enable_stdio_usb(neat 0)
pico_enable_stdio_uart(neat 0)
target_link_libraries(neat pico_stdlib tinyusb_device tinyusb_board)
pico_add_extra_outputs(neat)
