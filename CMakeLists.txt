# Copyright © 2024 Jacob Curlin
# cgx engine

include(FetchContent)

cmake_minimum_required(VERSION 3.6.0)
project(cgx VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# OpenGL
find_package(OpenGL REQUIRED)

# external dependencies
add_subdirectory(external)

# cgx Static Library Creation
add_library(cgx STATIC "")
link_glad(cgx PUBLIC)
link_glfw(cgx PUBLIC)
link_glm(cgx PUBLIC)
link_imgui(cgx PUBLIC)
link_spdlog(cgx PUBLIC)
link_stb_image(cgx PUBLIC)
link_tinyobjloader(cgx PUBLIC)

set(SHADERS_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/data/shaders/")
set(ASSETS_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/data/assets/")
set(FONTS_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/data/fonts/")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/utility/paths.h.in
        ${CMAKE_CURRENT_SOURCE_DIR}/include/utility/paths.h
        @ONLY )


add_subdirectory(examples)

target_sources(cgx
    # core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/engine.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/hierarchy.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/item.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/window_manager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/time_system.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/physics_system.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/engine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/hierarchy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/item.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/window_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/time_system.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/physics_system.cpp

    #input
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/input/input_types.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/input/input_manager.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/input/input_manager.cpp

    # ecs
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/system.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/component_array.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/component_registry.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/entity_registry.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/system_registry.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs/component_registry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs/entity_registry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs/system_registry.cpp

    # components
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/components/point_light.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/components/render.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/components/rigid_body.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/components/scene_node.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/components/transform.h

    # event
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/event/event.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/event/event_handler.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/event/event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/event/event_handler.cpp

    # events
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/event/events/engine_events.h

    # render
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/render/camera.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/render/framebuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/render/render_system.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/render/camera.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/render/framebuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/render/render_system.cpp

    # asset
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/asset.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/material.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/mesh.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/model.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/shader.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/texture.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/cubemap.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/asset_manager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/import/asset_importer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/import/asset_importer_image.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asset/import/asset_importer_obj.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/asset.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/material.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/mesh.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/model.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/shader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/texture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/cubemap.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/asset_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/import/asset_importer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/import/asset_importer_image.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asset/import/asset_importer_obj.cpp

    # gui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gui/gui_context.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gui/imgui_manager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gui/imgui_panel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gui/panels/profiler_panel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gui/panels/render_settings_panel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gui/panels/asset_panel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gui/panels/scene_panel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gui/panels/viewport_panel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gui/panels/properties_panel.h

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/gui_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/imgui_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/imgui_panel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/panels/profiler_panel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/panels/render_settings_panel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/panels/asset_panel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/panels/scene_panel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/panels/viewport_panel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/panels/properties_panel.cpp

    # geometry
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/geometry/primitive_mesh.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/geometry/primitive_mesh.cpp

    # scene
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/scene/scene.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/scene/scene_manager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/scene/node.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scene/scene.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scene/scene_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scene/node.cpp

    # utility
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/utility/logging.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/utility/math.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/utility/error.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utility/logging.cpp

    # glad
    PRIVATE
        # ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/glad.c
)

target_include_directories(cgx 
    PUBLIC 
        include/

    # SYSTEM
        external/
    
    PRIVATE 
        src/
)

get_target_property(TARGET_INCLUDES cgx INCLUDE_DIRECTORIES)
message(STATUS "Include directories for cgx: ${TARGET_INCLUDES}")


target_link_libraries(cgx PUBLIC
    # glfw
    OpenGL::GL
    # imgui
    # spdlog::spdlog_header_only
    # glm::glm-header-only
)

# copy assets to build directory
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data/shaders
        ${SHADERS_DIRECTORY}
)

# copy assets to build directory
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data/assets
        ${ASSETS_DIRECTORY}
)

# copy fonts to build directory
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data/fonts
        ${FONTS_DIRECTORY}
)
