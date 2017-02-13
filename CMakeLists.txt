cmake_minimum_required(VERSION 3.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

include(cmake/scripts/DownloadProject.cmake)
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/glfw-src)
    download_project(PROJ glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.2.1
            ${UPDATE_DISCONNECTED_IF_AVAILABLE}
            )
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
else()
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glfw-src ${CMAKE_CURRENT_BINARY_DIR}/glfw-build)
endif()

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/glbinding-src)
    download_project(PROJ glbinding
            GIT_REPOSITORY https://github.com/cginternals/glbinding.git
            GIT_TAG v2.1.1
            ${UPDATE_DISCONNECTED_IF_AVAILABLE}
            )
    add_subdirectory(${glbinding_SOURCE_DIR} ${glbinding_BINARY_DIR})
else()
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glbinding-src ${CMAKE_CURRENT_BINARY_DIR}/glbinding-build)
endif()
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/spdlog-src)
    download_project(PROJ spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG master
            ${UPDATE_DISCONNECTED_IF_AVAILABLE}
            )
else()
    set(spdlog_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/spdlog-src")
endif()
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/glm-src)
    download_project(PROJ glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG 0.9.8.4
            ${UPDATE_DISCONNECTED_IF_AVAILABLE}
            )
else()
    set(glm_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/glm-src")
endif()

add_subdirectory(src)