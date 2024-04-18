# Copyright © 2024 Jacob Curlin
# setup external dependencies

include(FetchContent)

# =====================================================
# glad

macro(link_opengl TARGET ACCESS)
    find_package(OpenGL REQUIRED)
    if (OpenGL_FOUND)
        target_include_directories(${TARGET} ${ACCESS} ${OPENGL_INCLUDE_DIR})

        target_link_libraries(${TARGET} ${ACCESS} OpenGL::GL)

        add_dependencies(${TARGET} OpenGL::GL)
        message(STATUS " > [dependency] 'OpenGL' found and linked to ${TARGET} : ${OPENGL_INCLUDE_DIR}")
   else()
        message(FATAL_ERROR " > [build failure] OpenGL not found on this system")
    endif()
endmacro()

# =====================================================
# glad

# glad config settings
set(GLAD_OUT_DIR        "${CMAKE_CURRENT_BINARY_DIR}" CACHE STRING "Output directory" FORCE)
set(GLAD_PROFILE        "core" CACHE STRING "OpenGL profile" FORCE)
set(GLAD_API            "gl=3.3" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means latest" FORCE)
set(GLAD_GENERATOR      "c" CACHE STRING "Language to generate the binding for" FORCE)
set(GLAD_EXTENSIONS     "" CACHE STRING "Path to extensions file or comma separated list of extensions, if missing all extensions are included" FORCE)
set(GLAD_SPEC           "gl" CACHE STRING "Name of the spec" FORCE)
set(GLAD_ALL_EXTENSIONS OFF CACHE BOOL "Include all extensions instead of those specified by GLAD_EXTENSIONS" FORCE)
set(GLAD_NO_LOADER      OFF CACHE BOOL "No loader" FORCE)
set(GLAD_REPRODUCIBLE   OFF CACHE BOOL "Reproducible build" FORCE)

macro(link_glad TARGET ACCESS)
    FetchContent_Declare(
            glad
            GIT_REPOSITORY https://github.com/Dav1dde/glad.git
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad"
            BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/glad"
    )

    FetchContent_GetProperties(glad)
    if (NOT glad_POPULATED)
        FetchContent_Populate(glad)
        add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif ()

    target_include_directories(${TARGET} ${ACCESS} ${glad_SOURCE_DIR})
    target_link_libraries(${TARGET} ${ACCESS} glad)


    add_dependencies(${TARGET} glad)
    message(STATUS " > [dependency] 'glad' loaded          : ${glad_SOURCE_DIR}")
endmacro()

# =====================================================
#glm

macro(link_glm TARGET ACCESS)

    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG a2844eede81f92b7dfb327f831c0bc0dbb273078 # 1.0.2
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glm"
            BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/glm"
    )
    FetchContent_GetProperties(glm)

    if (NOT glm_POPULATED)
        FetchContent_Populate(glm)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${glm_SOURCE_DIR})
    message(STATUS " > [dependency] 'glm' loaded           : ${glm_SOURCE_DIR}")
endmacro()

# =====================================================
# glfw

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "build examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "build tests" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "build docs" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "configure an install" FORCE)

macro (link_glfw TARGET ACCESS)

    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG b35641f4a3c62aa86a0b3c983d163bc0fe36026d # 04/12/24
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw"
            BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/glfw"
    )
    FetchContent_GetProperties(glfw)

    if (NOT glfw_POPULATED)
        FetchContent_Populate(glfw)
        add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${glfw_SOURCE_DIR}/include)
    target_link_libraries(${TARGET} ${ACCESS} glfw)

    add_dependencies(${TARGET} glfw)
    message(STATUS " > [dependency] 'glfw' loaded          : ${glfw_SOURCE_DIR}")
endmacro()

# =====================================================
# spdlog

macro(link_spdlog TARGET ACCESS)

    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG a2b4262090fd3f005c2315dcb5be2f0f1774a005
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog"
            BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/spdlog"
    )

    FetchContent_GetProperties(spdlog)
    if (NOT spdlog_POPULATED)
        FetchContent_Populate(spdlog)
        add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${spdlog_SOURCE_DIR}/include)
    target_link_libraries(${TARGET} ${ACCESS} spdlog::spdlog_header_only)

    add_dependencies(${TARGET} spdlog)
    message(STATUS " > [dependency] 'spdlog' loaded : ${spdlog_SOURCE_DIR}")
endmacro()

# =====================================================
# imgui

macro(link_imgui TARGET ACCESS)
    FetchContent_Declare(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG docking
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui"
            BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/imgui"
    )
    FetchContent_GetProperties(imgui)
    if (NOT imgui_POPULATED)
        FetchContent_Populate(imgui)
        set(IMGUI_SOURCES
                ${imgui_SOURCE_DIR}/imgui.cpp
                ${imgui_SOURCE_DIR}/imgui_demo.cpp
                ${imgui_SOURCE_DIR}/imgui_draw.cpp
                ${imgui_SOURCE_DIR}/imgui_widgets.cpp
                ${imgui_SOURCE_DIR}/imgui_tables.cpp
                ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
                ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        )
        add_library(imgui STATIC ${IMGUI_SOURCES})
    endif()

    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    target_link_libraries(imgui PRIVATE glfw)

    target_include_directories(${TARGET} ${ACCESS} ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    target_link_libraries(${TARGET} ${ACCESS} imgui)

    add_dependencies(${TARGET} imgui)
    message(STATUS " > [dependency] 'imgui' loaded         : ${imgui_SOURCE_DIR}")
endmacro()

# =====================================================
# stb (image)

macro(link_stb TARGET ACCESS)
    FetchContent_Declare(
            stb
            GIT_REPOSITORY https://github.com/nothings/stb.git
            GIT_TAG ae721c50eaf761660b4f90cc590453cdb0c2acd0
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/stb"
            BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/stb"
    )
    FetchContent_GetProperties(stb)

    if (NOT stb_POPULATED)
        FetchContent_Populate(stb)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${stb_SOURCE_DIR})
    message(STATUS " > [dependency] 'stb' loaded           : ${stb_SOURCE_DIR}")

endmacro()
# =====================================================
# tinyobjloader

macro(link_tinyobjloader TARGET ACCESS)
    FetchContent_Declare(
            tinyobjloader
            GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
            GIT_TAG cab4ad7254cbf7eaaafdb73d272f99e92f166df8
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/tinyobjloader"
            BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/tinyobjloader"
    )
    FetchContent_GetProperties(tinyobjloader)

    if (NOT tinyobjloader_POPULATED)
        FetchContent_Populate(tinyobjloader)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${tinyobjloader_SOURCE_DIR})
    message(STATUS " > [dependency] 'tinyobjloader' loaded : ${tinyobjloader_SOURCE_DIR}")
endmacro()