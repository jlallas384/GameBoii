cmake_minimum_required(VERSION 3.10)
project(GameBoii)
set(CMAKE_CXX_STANDARD 20)


include_directories(src)
add_subdirectory(tests)

add_executable(${CMAKE_PROJECT_NAME})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    src/address_bus.cpp
    src/address_bus.h
    src/application.cpp
    src/application.h
    src/cartridge.cpp
    src/cartridge.h
    src/game_boy.cpp
    src/game_boy.h
    src/irq_handler.cpp
    src/irq_handler.h
    src/joypad.cpp
    src/joypad.h
    src/main.cpp
    src/sdl_lcd.cpp
    src/sdl_lcd.h
    src/timers.cpp
    src/timers.h
    src/cpu/cpu.cpp
    src/cpu/cpu.h
    src/cpu/alu.cpp
    src/cpu/alu.h
    src/mapper/mapper0.cpp
    src/mapper/mapper0.h
    src/mapper/mapper1.cpp
    src/mapper/mapper1.h
    src/mapper/mapper2.cpp
    src/mapper/mapper2.h
    src/mapper/mapper3.cpp
    src/mapper/mapper3.h
    src/mapper/mapper5.cpp
    src/mapper/mapper5.h
    src/ppu/ppu.cpp
    src/ppu/ppu.h
    src/ppu/tile.cpp
    src/ppu/tile.h
    src/ppu/object_layer.cpp   
    src/ppu/object_layer.h
    src/ppu/lcd
    src/utils.h
)


configure_file(cgb_boot.bin cgb_boot.bin COPYONLY)


find_package(SDL2 REQUIRED)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES})
