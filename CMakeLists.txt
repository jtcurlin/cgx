# Copyright © 2024 Jacob Curlin

cmake_minimum_required(VERSION 3.19.0)
project(cgx VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external)
add_subdirectory(examples)

option(FETCH_EXTERNAL_DEPENDENCIES "Fetch dependencies from external repositories if not present" ON)
option(PREFER_BUNDLED_DEPENDENCIES "Prefer to use bundled versions of dependencies rather than them fetching externally" ON)

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

set(EXTERNAL_SOURCE_DIR "${CMAKE_SOURCE_DIR}/include")
set(EXTERNAL_BINARY_DIR "${CMAKE_SOURCE_DIR}/include")

# configure paths.h before setting up dependent targets
set(DATA_DIRECTORY "${CMAKE_BINARY_DIR}/data")
configure_file(
        ${INCLUDE_DIR}/utility/paths.h.in
        ${INCLUDE_DIR}/utility/paths.h
        @ONLY
)

file(GLOB_RECURSE PUBLIC_HEADERS "${INCLUDE_DIR}/*.h")
file(GLOB_RECURSE PRIVATE_SOURCES "${SOURCE_DIR}/*.cpp")

add_library(cgx STATIC "") # create static engine library

link_opengl(cgx PUBLIC)
link_spdlog(cgx PUBLIC)
link_glad(cgx PUBLIC)
link_glfw(cgx PUBLIC)
link_glm(cgx PUBLIC)
link_imgui(cgx PUBLIC)
link_stb(cgx PUBLIC)
link_tinyobjloader(cgx PUBLIC)
link_tinygltfloader(cgx PUBLIC)
# link_openal(cgx PUBLIC)
# link_libsndfile(cgx PUBLIC)

target_sources(cgx PUBLIC ${PUBLIC_HEADERS} PRIVATE ${PRIVATE_SOURCES})
target_include_directories(cgx PUBLIC ${INCLUDE_DIR} ${EXTERNAL_SOURCE_DIR} PRIVATE ${SOURCE_DIR})

# copy data (assets, fonts, shaders, etc.) into to build directory
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        ${DATA_DIRECTORY}
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sandbox)
