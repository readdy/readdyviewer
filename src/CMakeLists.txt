project(readdyviewer)

set(CMAKE_CXX_STANDARD 14)

find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED)

set(SOURCE_FILES main.cpp common.h)
set(SOURCE_FILES_43 gl43/common43.h gl43/Viewer.cpp gl43/Viewer.h gl43/Camera.cpp gl43/Camera.h
        gl43/ShaderProgram.cpp gl43/ShaderProgram.h gl43/PointSprite.cpp gl43/PointSprite.h
        gl43/Trajectory.cpp gl43/Trajectory.h gl43/Framing.cpp gl43/Framing.h
        gl43/Texture.cpp gl43/Texture.h gl43/LightArrangement.cpp gl43/LightArrangement.h)

set(SOURCE_FILES_30 gl30/common30.h)

if (BUILD_PYTHON_LIBRARY)
    pybind11_add_module(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCE_FILES_43} ${SOURCE_FILES_30})
else ()
    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCE_FILES_43} ${SOURCE_FILES_30})
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
        ${spdlog_SOURCE_DIR}/include ${glfw_SOURCE_DIR}/include ${PNG_INCLUDE_DIRS} ${glm_SOURCE_DIR}
        )

target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${GLFW_LIBRARIES} ${PNG_LIBRARIES} ${OPENGL_LIBRARIES} glbinding::glbinding)

if (HIDDEN_FRANK_MODE)
    if(BUILD_PYTHON_LIBRARY)
        target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE HIDDEN_FRANK_MODE=1 PYLIB=1)
    else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE HIDDEN_FRANK_MODE=1 PYLIB=0)
    endif()
else ()
    if(BUILD_PYTHON_LIBRARY)
        target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE HIDDEN_FRANK_MODE=0 PYLIB=1)
    else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE HIDDEN_FRANK_MODE=0 PYLIB=0)
    endif()
endif ()

file(GLOB_RECURSE shaders gl43/shaders FOLLOW_SYMLINKS *.glsl)
foreach (shader ${shaders})
    file(RELATIVE_PATH relative_shader ${CMAKE_CURRENT_SOURCE_DIR} ${shader})
    configure_file(${shader} "${CMAKE_CURRENT_BINARY_DIR}/${relative_shader}")
endforeach ()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
