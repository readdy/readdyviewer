cmake_minimum_required(VERSION 3.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

include(cmake/Modules/DownloadProject.cmake)
download_project(PROJ glbinding
        GIT_REPOSITORY https://github.com/cginternals/glbinding.git
        GIT_TAG v2.1.1
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )
add_subdirectory(${glbinding_SOURCE_DIR} ${glbinding_BINARY_DIR})
download_project(PROJ spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )

download_project(PROJ glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.2.1
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )
add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})

add_subdirectory(src)