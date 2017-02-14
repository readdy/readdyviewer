project(readdyviewer)

set(CMAKE_CXX_STANDARD 14)

find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED)

set(SOURCE_FILES main.cpp common.h
        Viewer.cpp Viewer.h
        Camera.cpp Camera.h
        ShaderProgram.cpp ShaderProgram.h
        PointSprite.cpp PointSprite.h
        Trajectory.cpp Trajectory.h
        Framing.cpp Framing.h
        Texture.cpp Texture.h)

add_executable(readdyviewer ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
        ${spdlog_SOURCE_DIR}/include ${glfw_SOURCE_DIR}/include ${PNG_INCLUDE_DIRS} ${glm_SOURCE_DIR}
        )

target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${GLFW_LIBRARIES} ${PNG_LIBRARIES} ${OPENGL_LIBRARIES} glbinding::glbinding)

if(HIDDEN_FRANK_MODE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE HIDDEN_FRANK_MODE=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE HIDDEN_FRANK_MODE=0)
endif()

file(GLOB_RECURSE shaders shaders FOLLOW_SYMLINKS *.glsl)
foreach (shader ${shaders})
    file(RELATIVE_PATH relative_shader ${CMAKE_CURRENT_SOURCE_DIR} ${shader})
    configure_file(${shader} "${CMAKE_CURRENT_BINARY_DIR}/${relative_shader}")
endforeach ()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
