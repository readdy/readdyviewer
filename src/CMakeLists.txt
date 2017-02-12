project(readdyviewer)

set(CMAKE_CXX_STANDARD 14)

find_package (PNG REQUIRED)
find_package (GLM REQUIRED)
find_package (OpenGL REQUIRED)

set(SOURCE_FILES main.cpp common.h)
add_executable(readdyviewer ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${spdlog_SOURCE_DIR}/include ${glfw_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/glcorew ${PNG_INCLUDE_DIRS} ${GLM_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${PNG_LIBRARIES} ${OPENGL_LIBRARIES} glbinding::glbinding)