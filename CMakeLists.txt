# Copyright © 2024 Jacob Curlin

cmake_minimum_required(VERSION 3.19.0)
project(cgx VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(USE_SOURCE_DIR_DATA "Configure the engine to use the data within the source rather than the build directory" ON)
option(FETCH_EXTERNAL_DEPENDENCIES "Fetch dependencies from external repositories if not present" ON)
option(PREFER_BUNDLED_DEPENDENCIES "Prefer to use bundled versions of dependencies rather than them fetching externally" ON)

# project source/dir paths
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")

# configure paths.h before setting up dependent targets
if (USE_SOURCE_DIR_DATA)
    set(DATA_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
else ()
    set(DATA_DIRECTORY "${CMAKE_BINARY_DIR}/data")
endif ()
configure_file(
        ${INCLUDE_DIR}/utility/paths.h.in
        ${INCLUDE_DIR}/utility/paths.h
        @ONLY
)

add_library(cgx STATIC "") # create static engine library

# link dependencies
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies.cmake")
link_opengl(cgx PUBLIC)
link_spdlog(cgx PUBLIC)
link_glad(cgx PUBLIC)
link_glfw(cgx PUBLIC)
link_glm(cgx PUBLIC)
link_imgui(cgx PUBLIC)
link_stb(cgx PUBLIC)
link_tinyobjloader(cgx PUBLIC)
link_tinygltfloader(cgx PUBLIC)
link_imguifiledialog(cgx PUBLIC)

target_sources(cgx PRIVATE
        ${SOURCE_DIR}/asset/import/asset_importer.cpp
        ${SOURCE_DIR}/asset/import/asset_importer_image.cpp
        ${SOURCE_DIR}/asset/import/asset_importer_obj.cpp
        ${SOURCE_DIR}/asset/asset.cpp
        ${SOURCE_DIR}/asset/asset_manager.cpp
        ${SOURCE_DIR}/asset/cubemap.cpp
        ${SOURCE_DIR}/asset/material.cpp
        ${SOURCE_DIR}/asset/mesh.cpp
        ${SOURCE_DIR}/asset/model.cpp
        ${SOURCE_DIR}/asset/pbr_material.cpp
        ${SOURCE_DIR}/asset/phong_material.cpp
        ${SOURCE_DIR}/asset/shader.cpp
        ${SOURCE_DIR}/asset/texture.cpp
        ${SOURCE_DIR}/core/systems/camera_system.cpp
        ${SOURCE_DIR}/core/systems/control_system.cpp
        ${SOURCE_DIR}/core/systems/hierarchy_system.cpp
        ${SOURCE_DIR}/core/systems/physics_system.cpp
        ${SOURCE_DIR}/core/systems/time_system.cpp
        ${SOURCE_DIR}/core/systems/transform_system.cpp
        ${SOURCE_DIR}/core/engine.cpp
        ${SOURCE_DIR}/core/event_handler.cpp
        ${SOURCE_DIR}/core/hierarchy.cpp
        ${SOURCE_DIR}/core/input_manager.cpp
        ${SOURCE_DIR}/core/item.cpp
        ${SOURCE_DIR}/core/window_manager.cpp
        ${SOURCE_DIR}/ecs/component_registry.cpp
        ${SOURCE_DIR}/ecs/ecs_manager.cpp
        ${SOURCE_DIR}/ecs/entity_registry.cpp
        ${SOURCE_DIR}/ecs/system_registry.cpp
        ${SOURCE_DIR}/gui/panels/asset_panel.cpp
        ${SOURCE_DIR}/gui/panels/dialog_panel.cpp
        ${SOURCE_DIR}/gui/panels/input_panel.cpp
        ${SOURCE_DIR}/gui/panels/profiler_panel.cpp
        ${SOURCE_DIR}/gui/panels/properties_panel.cpp
        ${SOURCE_DIR}/gui/panels/render_settings_panel.cpp
        ${SOURCE_DIR}/gui/panels/scene_panel.cpp
        ${SOURCE_DIR}/gui/panels/viewport_panel.cpp
        ${SOURCE_DIR}/gui/gui_context.cpp
        ${SOURCE_DIR}/gui/imgui_manager.cpp
        ${SOURCE_DIR}/gui/imgui_panel.cpp
        ${SOURCE_DIR}/render/camera.cpp
        ${SOURCE_DIR}/render/framebuffer.cpp
        ${SOURCE_DIR}/render/render_system.cpp
        ${SOURCE_DIR}/scene/node.cpp
        ${SOURCE_DIR}/scene/camera_node.cpp
        ${SOURCE_DIR}/scene/mesh_node.cpp
        ${SOURCE_DIR}/scene/root_node.cpp
        ${SOURCE_DIR}/scene/scene.cpp
        ${SOURCE_DIR}/scene/scene_importer.cpp
        ${SOURCE_DIR}/scene/scene_manager.cpp
        ${SOURCE_DIR}/utility/logging.cpp
        ${SOURCE_DIR}/utility/primitive_mesh.cpp
)

target_include_directories(cgx PUBLIC ${INCLUDE_DIR} ${EXTERNAL_DIR} PRIVATE ${SOURCE_DIR})

if (NOT USE_SOURCE_DIR_DATA)
    # copy data (assets, fonts, shaders, etc.) into to build directory
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data
            ${DATA_DIRECTORY}
    )
endif ()

add_subdirectory(examples)

# for visual studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sandbox)

