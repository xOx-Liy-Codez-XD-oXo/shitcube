cmake_minimum_required(VERSION 4.0)
project(shitcube LANGUAGES C)

if(NOT (${CMAKE_C_COMPILER_ID} STREQUAL "AppleClang" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang"))
  set(CMAKE_C_STANDARD 89)
  set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

add_executable(shitcube
  gekko/dol.c
  gekko/elf.c
  gekko/hardware_reg.c
  gekko/instruction.c
  gekko/interpreter.c
  gekko/memory.c
  gekko/opcode.c
  global/endian.c
  main.c
)

target_compile_definitions(shitcube PRIVATE LE)
target_compile_options(shitcube PRIVATE -Wall -Wpedantic)
