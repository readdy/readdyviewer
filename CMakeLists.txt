cmake_minimum_required(VERSION 3.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(HIDDEN_FRANK_MODE OFF CACHE BOOL "hidden frank mode!")
set(BUILD_PYTHON_LIBRARY ON CACHE BOOL "build python library")

set(READDYVIEWER_ROOT ${CMAKE_SOURCE_DIR})
set(READDYVIEWER_BINARY_ROOT ${CMAKE_BINARY_DIR})

include(cmake/scripts/DownloadProject.cmake)
if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/glfw-src)
    download_project(PROJ glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.2.1
            ${UPDATE_DISCONNECTED_IF_AVAILABLE}
            )
    set(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
    set(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
    set(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
else ()
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glfw-src ${CMAKE_CURRENT_BINARY_DIR}/glfw-build)
endif ()

set (OPTION_BUILD_EXAMPLES  OFF CACHE BOOL "Don't need the examples")
set (OPTION_BUILD_TESTS     OFF CACHE BOOL "Don't need to test")
set (OPTION_BUILD_GPU_TESTS OFF CACHE BOOL "Don't need to test")
set (OPTION_BUILD_TOOLS     OFF CACHE BOOL "Don't need the tools")
if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/glbinding-src)
    download_project(PROJ glbinding
            GIT_REPOSITORY https://github.com/cginternals/glbinding.git
            GIT_TAG v2.1.3
            ${UPDATE_DISCONNECTED_IF_AVAILABLE}
            )
    add_subdirectory(${glbinding_SOURCE_DIR} ${glbinding_BINARY_DIR})
else ()
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glbinding-src ${CMAKE_CURRENT_BINARY_DIR}/glbinding-build)
endif ()

add_subdirectory(libs/pybind11)
set(BUILD_SHARED_LIBS ON)
add_subdirectory(libs/fmt)
add_subdirectory(libs/spdlog)
add_subdirectory(libs/glm)

add_subdirectory(readdyviewer/resources/shaders)
add_subdirectory(readdyviewer/resources/textures)

add_subdirectory(src)
