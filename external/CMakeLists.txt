# Copyright © 2024 Jacob Curlin
# setup external dependencies

include(FetchContent)

macro(check_directory DIR)
    if(EXISTS "${DIR}")
        file(GLOB contents "${DIR}/*")
        list(LENGTH contents contents_count)

        if(contents_count EQUAL 0)
            message(NOTICE " > Directory exists but is empty, removing: ${DIR}")
            file(REMOVE_RECURSE "${DIR}")
        endif()
    endif()
endmacro()


# =====================================================
# opengl

macro(link_opengl TARGET ACCESS)
    find_package(OpenGL REQUIRED)
    if (OpenGL_FOUND)
        target_include_directories(${TARGET} ${ACCESS} ${OPENGL_INCLUDE_DIR})
        target_link_libraries(${TARGET} ${ACCESS} OpenGL::GL)
        add_dependencies(${TARGET} OpenGL::GL)
        message(STATUS " >>  [dependency : opengl] loaded & linked successfully")
   else()
        message(FATAL_ERROR " >  [dependency : opengl] not found on this system")
    endif()
endmacro()

# =====================================================
# glad

macro(link_glad TARGET ACCESS)
    set(glad_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")

    if(EXISTS "${glad_SOURCE_DIR}/src/glad.c")
        message(STATUS " > [dependency: glad] Using bundled GLAD library @ ${glad_SOURCE_DIR}")

        add_library(glad STATIC "${glad_SOURCE_DIR}/src/glad.c")
        target_include_directories(glad PUBLIC "${glad_SOURCE_DIR}/include")

        target_link_libraries(${TARGET} ${ACCESS} glad)
        target_include_directories(${TARGET} ${ACCESS} "${glad_SOURCE_DIR}/include")

        add_dependencies(${TARGET} glad)
    else()
        message(FATAL_ERROR " > [dependency: glad] GLAD directory not found or incomplete in ${glad_SOURCE_DIR}")
    endif()
endmacro()


# =====================================================
#glm

macro(link_glm TARGET ACCESS)
    set(glm_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glm")
    set(glm_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/glm")

    check_directory("${glm_SOURCE_DIR}")
    if (PREFER_BUNDLED_DEPENDENCIES AND EXISTS "${glm_SOURCE_DIR}")
        message(STATUS " >  [dependency : glm] using bundled library @ ${glm_SOURCE_DIR}")
    elseif (FETCH_EXTERNAL_DEPENDENCIES)
        FetchContent_Declare(
                glm
                GIT_REPOSITORY https://github.com/g-truc/glm.git
                GIT_TAG 1.0.1
                SOURCE_DIR "${glm_SOURCE_DIR}"
                BINARY_DIR "${glm_BINARY_DIR}"
        )
        FetchContent_GetProperties(glm)

        if (NOT glm_POPULATED)
            FetchContent_Populate(glm)
        endif()
    else()
        file(REMOVE_RECURSE "${glm_SOURCE_DIR}")
        message(FATAL_ERROR "!> [dependency : glm] external fetching disabled, glm not found @ ${glm_SOURCE_DIR}")
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${glm_SOURCE_DIR})
    message(STATUS " >> [dependency : glm] loaded successfully")
endmacro()

# =====================================================
# glfw

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "build examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "build tests" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "build docs" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "configure an install" FORCE)

macro (link_glfw TARGET ACCESS)
    set(glfw_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw")
    set(glfw_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/glfw")

    check_directory("${glfw_SOURCE_DIR}")
    if (PREFER_BUNDLED_DEPENDENCIES AND EXISTS "${glfw_SOURCE_DIR}")
        message(STATUS " >  [dependency : glfw] using bundled library @ ${glfw_SOURCE_DIR}")
    elseif (FETCH_EXTERNAL_DEPENDENCIES)
        FetchContent_Declare(
                glfw
                GIT_REPOSITORY https://github.com/glfw/glfw.git
                GIT_TAG 3.4
                SOURCE_DIR "${glfw_SOURCE_DIR}"
                BINARY_DIR "${glfw_BINARY_DIR}"
        )
        FetchContent_GetProperties(glfw)

        if (NOT glfw_POPULATED)
            FetchContent_Populate(glfw)
        endif()
    else()
        file(REMOVE_RECURSE "${glfw_SOURCE_DIR}")
        message(FATAL_ERROR "!> [dependency : glfw] external fetching disabled, glfw not found @ ${glfw_SOURCE_DIR}")
    endif()

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
    target_include_directories(${TARGET} ${ACCESS} ${glfw_SOURCE_DIR}/include)
    target_link_libraries(${TARGET} ${ACCESS} glfw)

    add_dependencies(${TARGET} glfw)
    message(STATUS " >> [dependency : glfw] loaded successfully")
endmacro()

# =====================================================
# spdlog


macro(link_spdlog TARGET ACCESS)
    set(spdlog_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog")
    set(spdlog_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/spdlog")

    check_directory("${spdlog_SOURCE_DIR}")
    if (PREFER_BUNDLED_DEPENDENCIES AND EXISTS "${spdlog_SOURCE_DIR}")
        message(STATUS " >  [dependency : spdlog] using bundled library @ ${spdlog_SOURCE_DIR}")
    elseif (FETCH_EXTERNAL_DEPENDENCIES)
        FetchContent_Declare(
                spdlog
                GIT_REPOSITORY https://github.com/gabime/spdlog.git
                GIT_TAG v1.13.0
                SOURCE_DIR "${spdlog_SOURCE_DIR}"
                BINARY_DIR "${spdlog_BINARY_DIR}"
        )

        FetchContent_GetProperties(spdlog)
        if (NOT spdlog_POPULATED)
            FetchContent_Populate(spdlog)
        endif()
    else()
        file(REMOVE_RECURSE "${spdlog_SOURCE_DIR}")
        message(FATAL_ERROR "!> [dependency : spdlog] external fetching disabled, spdlog not found @ ${spdlog_SOURCE_DIR}")
    endif()

    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR} EXCLUDE_FROM_ALL)
    target_include_directories(${TARGET} ${ACCESS} ${spdlog_SOURCE_DIR}/include)
    target_link_libraries(${TARGET} ${ACCESS} spdlog::spdlog_header_only)

    add_dependencies(${TARGET} spdlog)
    message(STATUS " >> [dependency: spdlog] loaded successfully")
endmacro()

# =====================================================
# imgui

macro(link_imgui TARGET ACCESS)
    set(imgui_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
    set(imgui_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/imgui")

    check_directory("${imgui_SOURCE_DIR}")
    if (PREFER_BUNDLED_DEPENDENCIES AND EXISTS "${imgui_SOURCE_DIR}")
        message(STATUS " >  [dependency : imgui] using bundled library @ ${imgui_SOURCE_DIR}")
    elseif (FETCH_EXTERNAL_DEPENDENCIES)
        FetchContent_Declare(
                imgui
                GIT_REPOSITORY https://github.com/ocornut/imgui.git
                GIT_TAG v1.90.5-docking
                SOURCE_DIR "${imgui_SOURCE_DIR}"
                BINARY_DIR "${imgui_BINARY_DIR}"
        )
        FetchContent_GetProperties(imgui)
        if (NOT imgui_POPULATED)
            FetchContent_Populate(imgui)
        endif()
    else()
        file(REMOVE_RECURSE "${imgui_SOURCE_DIR}")
        message(FATAL_ERROR "!> [dependency : imgui] external fetching disabled, imgui not found @ ${imgui_SOURCE_PATH}")
    endif()

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

    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    target_link_libraries(imgui PRIVATE glfw)

    target_include_directories(${TARGET} ${ACCESS} ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
    target_link_libraries(${TARGET} ${ACCESS} imgui)

    add_dependencies(${TARGET} imgui)
    message(STATUS " >> [dependency : imgui] loaded successfully")
endmacro()

# =====================================================
# stb (image)

macro(link_stb TARGET ACCESS)
    set(stb_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/stb")
    set(stb_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/stb")

    check_directory("${stb_SOURCE_DIR}")
    if (PREFER_BUNDLED_DEPENDENCIES AND EXISTS "${stb_SOURCE_DIR}")
        message(STATUS " >  [dependency : stb] using bundled library @ ${stb_SOURCE_DIR}")
    elseif (FETCH_EXTERNAL_DEPENDENCIES)
        FetchContent_Declare(
                stb
                GIT_REPOSITORY https://github.com/nothings/stb.git
                GIT_TAG ae721c50eaf761660b4f90cc590453cdb0c2acd0
                SOURCE_DIR "${stb_SOURCE_DIR}"
                BINARY_DIR "${stb_BINARY_DIR}"
        )
        FetchContent_GetProperties(stb)

        if (NOT stb_POPULATED)
            FetchContent_Populate(stb)
        endif()
    else()
        file(REMOVE_RECURSE "${stb_SOURCE_DIR}")
        message(FATAL_ERROR "!> [dependency : stb]          external fetching disabled, stb not found @ ${stb_SOURCE_DIR}")
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${stb_SOURCE_DIR})
    message(STATUS " >> [dependency : stb] loaded successfully")

endmacro()

# =====================================================
# tinyobjloader

macro(link_tinyobjloader TARGET ACCESS)
    set(tinyobjloader_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/tinyobjloader")
    set(tinyobjloader_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/tinyobjloader")

    check_directory("${tinyobjloader_SOURCE_DIR}")
    if (PREFER_BUNDLED_DEPENDENCIES AND EXISTS "${tinyobjloader_SOURCE_DIR}")
        message(STATUS " >  [dependency : tinyobjloader] using bundled library @ ${tinyobjloader_SOURCE_DIR}")
    elseif (FETCH_EXTERNAL_DEPENDENCIES)
        FetchContent_Declare(
                tinyobjloader
                GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
                GIT_TAG cab4ad7254cbf7eaaafdb73d272f99e92f166df8
                SOURCE_DIR "${tinyobjloader_SOURCE_DIR}"
                BINARY_DIR "${tinyobjloader_BINARY_DIR}"
        )
        FetchContent_GetProperties(tinyobjloader)

        if (NOT tinyobjloader_POPULATED)
            FetchContent_Populate(tinyobjloader)
        endif()
    else()
        file(REMOVE_RECURSE "${tinyobjloader_SOURCE_DIR}")
        message(FATAL_ERROR "!> [dependency : tinyobjloader] external fetching disabled, tinyobjloader not found @ ${tinyobjloader_SOURCE_DIR}")
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${tinyobjloader_SOURCE_DIR})
    message(STATUS " >> [dependency : tinyobjloader] loaded successfully")
endmacro()

# =====================================================
# tinygltfloader

macro(link_tinygltfloader TARGET ACCESS)
    set(tinygltfloader_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/tinygltfloader")
    set(tinygltfloader_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/tinygltfloader")

    check_directory("${tinygltfloader_SOURCE_DIR}")
    if (PREFER_BUNDLED_DEPENDENCIES AND EXISTS "${tinygltfloader_SOURCE_DIR}")
        message(STATUS " >  [dependency : tinygltfloader] using bundled library @ ${tinygltfloader_SOURCE_DIR}")
    elseif (FETCH_EXTERNAL_DEPENDENCIES)
        FetchContent_Declare(
                tinygltfloader
                GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
                GIT_TAG v2.8.21
                SOURCE_DIR "${tinygltfloader_SOURCE_DIR}"
                BINARY_DIR "${tinygltfloader_BINARY_DIR}"
        )
        FetchContent_GetProperties(tinygltfloader)

        if (NOT tinygltfloader_POPULATED)
            FetchContent_Populate(tinygltfloader)
        endif()
    else()
        file(REMOVE_RECURSE "${tinygltfloader_SOURCE_DIR}")
        message(FATAL_ERROR "!> [dependency : tinygltfloader] external fetching disabled, tinygltfloader not found @ ${tinygltfloader_SOURCE_DIR}")
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${tinygltfloader_SOURCE_DIR})
    message(STATUS " >> [dependency : tinygltfloader] loaded successfully")
endmacro()

# =====================================================
# openAL

set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)

macro(link_openal TARGET ACCESS)
    set(openal-soft_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/openal-soft")
    set(openal-soft_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/openal-soft")

    check_directory("${openal-soft_SOURCE_DIR}")
    if (PREFER_BUNDLED_DEPENDENCIES AND EXISTS "${openal-soft_SOURCE_DIR}")
        message(STATUS " >  [dependency : openal-soft] using bundled library @ ${openal-soft_SOURCE_DIR}")
    elseif (FETCH_EXTERNAL_DEPENDENCIES)
        FetchContent_Declare(
                openal
                GIT_REPOSITORY https://github.com/kcat/openal-soft.git
                GIT_TAG 1.23.1
                SOURCE_DIR "${openal-soft_SOURCE_DIR}"
                BINARY_DIR "${openal-soft_BINARY_DIR}"
        )
        FetchContent_GetProperties(openal-soft)
        if (NOT openal-soft_POPULATED)
            FetchContent_Populate(openal)
        endif()
    else()
        file(REMOVE_RECURSE "${openal-soft_SOURCE_DIR}")
        message(FATAL_ERROR "!> [dependency : openal-soft] external fetching disabled, openal-soft not found @ ${openal-soft_SOURCE_DIR}")
    endif()

    add_subdirectory(${openal-soft_SOURCE_DIR} ${openal-soft_BINARY_DIR})
    target_link_libraries(${TARGET} ${ACCESS} OpenAL)

    add_dependencies(${TARGET} OpenAL)
    message(STATUS " >> [dependency : openal-soft] loaded successfully")
endmacro()

# =====================================================
# libsndfile

set(LIBSNDFILE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LIBSNDFILE_TESTS OFF CACHE BOOL "" FORCE)
set(LIBSNDFILE_UTILS OFF CACHE BOOL "" FORCE)

macro(link_libsndfile TARGET ACCESS)
    set(libsndfile_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/libsndfile")
    set(libsndfile_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/libsndfile")

    check_directory("${libsndfile_SOURCE_DIR}")
    if (PREFER_BUNDLED_DEPENDENCIES AND EXISTS "${libsndfile_SOURCE_DIR}")
        message(STATUS " >  [dependency : libsndfile] using bundled library @ ${libsndfile_SOURCE_DIR}")
    elseif (FETCH_EXTERNAL_DEPENDENCIES)
        FetchContent_Declare(
                libsndfile
                GIT_REPOSITORY https://github.com/libsndfile/libsndfile.git
                GIT_TAG 1.2.2
                SOURCE_DIR "${libsndfile_SOURCE_DIR}"
                BINARY_DIR "${libsndfile_BINARY_DIR}"
        )
        FetchContent_GetProperties(libsndfile)
        if (NOT libsndfile_POPULATED)
            FetchContent_Populate(libsndfile)
        endif()
    else()
        file(REMOVE_RECURSE "${libsndfile_SOURCE_DIR}")
        message(FATAL_ERROR "!> [dependency : libsndfile] external fetching disabled, libsndfile not found @ ${libsndfile_SOURCE_DIR}")
    endif()

    add_subdirectory(${libsndfile_SOURCE_DIR} ${libsndfile_BINARY_DIR})
    target_link_libraries(${TARGET} ${ACCESS} sndfile)

    add_dependencies(${TARGET} sndfile)
    message(STATUS " >> [dependency : libsndfile] loaded successfully")
endmacro()




