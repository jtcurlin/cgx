# Copyright © 2024 Jacob Curlin


set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE PUBLIC_HEADERS "${INCLUDE_DIR}/*.h")
file(GLOB_RECURSE PRIVATE_SOURCES "${SOURCE_DIR}/*.cpp")

add_executable(sandbox ${PRIVATE_SOURCES})

target_include_directories(sandbox PUBLIC ${INCLUDE_DIR})

target_link_libraries(sandbox cgx)

add_dependencies(sandbox cgx)


