# Copyright © 2024 Jacob Curlin
# setup external dependencies

# =====================================================

# glad

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
    )

    FetchContent_GetProperties(glad)
    if (NOT glad_POPULATED)
        FetchContent_Populate(glad)
        add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif ()

    target_include_directories(${TARGET} ${ACCESS} ${glad_SOURCE_DIR})
    target_link_libraries(${TARGET} ${ACCESS} glad)

    message(STATUS "glad source directory: ${glad_SOURCE_DIR}")

    add_dependencies(${TARGET} glad)
endmacro()

# =====================================================

#glm

macro(link_glm TARGET ACCESS)

    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG a2844eede81f92b7dfb327f831c0bc0dbb273078 # 1.0.2
    )

    FetchContent_GetProperties(glm)

    if (NOT glm_POPULATED)
        FetchContent_Populate(glm)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${glm_SOURCE_DIR})
endmacro()

    # =====================================================

# glfw

#add_subdirectory(glfw)

macro (link_glfw TARGET ACCESS)

    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG b35641f4a3c62aa86a0b3c983d163bc0fe36026d # 04/12/24
    )
    FetchContent_GetProperties(glfw)

    if (NOT glfw_POPULATED)
        FetchContent_Populate(glfw)

        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "build examples" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "build tests" FORCE)
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "build docs" FORCE)
        set(GLFW_INSTALL OFF CACHE BOOL "configure an install" FORCE)

        add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)

        # set_target_properties(glfw PROPERTIES FOLDER ${PROJECT_NAME}/external)
    endif()
    message(STATUS "glfw source directory: ${glfw_SOURCE_DIR}")

    target_include_directories(${TARGET} ${ACCESS} ${glfw_SOURCE_DIR}/include)
    target_link_libraries(${TARGET} ${ACCESS} glfw)

    add_dependencies(${TARGET} glfw)
endmacro()

# =====================================================
# spdlog

macro(link_spdlog TARGET ACCESS)

    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG a2b4262090fd3f005c2315dcb5be2f0f1774a005
    )

    FetchContent_GetProperties(spdlog)
    if (NOT spdlog_POPULATED)
        FetchContent_Populate(spdlog)
        add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${spdlog_SOURCE_DIR}/include)
    target_link_libraries(${TARGET} ${ACCESS} spdlog::spdlog_header_only)

    add_dependencies(${TARGET} spdlog)
endmacro()

# =====================================================
# ImGui

macro(link_imgui TARGET ACCESS)

    FetchContent_Declare(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG docking
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

    message(STATUS "ImGui source directory: ${imgui_SOURCE_DIR}")

    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    target_link_libraries(imgui PRIVATE glfw)


    target_include_directories(${TARGET} ${ACCESS} ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    target_link_libraries(${TARGET} ${ACCESS} imgui)

    add_dependencies(${TARGET} imgui)
endmacro()

# =====================================================

macro(link_stb_image TARGET ACCESS)
    FetchContent_Declare(
            stb_image
            GIT_REPOSITORY https://github.com/nothings/stb.git
            GIT_TAG ae721c50eaf761660b4f90cc590453cdb0c2acd0
    )
    FetchContent_GetProperties(stb_image)

    if (NOT stb_image_POPULATED)
        FetchContent_Populate(stb_image)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${stb_image_SOURCE_DIR})

endmacro()

# =====================================================

macro(link_tinyobjloader TARGET ACCESS)
    FetchContent_Declare(
            tinyobjloader
            GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
            GIT_TAG cab4ad7254cbf7eaaafdb73d272f99e92f166df8
    )
    FetchContent_GetProperties(tinyobjloader)

    if (NOT tinyobjloader_POPULATED)
        FetchContent_Populate(tinyobjloader)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${tinyobjloader_SOURCE_DIR})

endmacro()