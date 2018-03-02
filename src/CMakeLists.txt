project(readdyviewer_binding)

set(CMAKE_CXX_STANDARD 17)

find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED)

set(SOURCE_FILES main.cpp common.h preprocessing.h
        Viewer.cpp Viewer.h
        Camera.cpp Camera.h
        ShaderProgram.cpp ShaderProgram.h
        PointSprite.cpp PointSprite.h
        Trajectory.cpp Trajectory.h
        Framing.cpp Framing.h
        Texture.cpp Texture.h
        LightArrangement.cpp LightArrangement.h
        Edges.cpp Edges.h
        TesselatedSphereSprite.cpp TesselatedSphereSprite.h)

if (BUILD_PYTHON_LIBRARY)
    pybind11_add_module(${PROJECT_NAME} ${SOURCE_FILES})
else ()
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
        ${spdlog_SOURCE_DIR}/include ${glfw_SOURCE_DIR}/include ${PNG_INCLUDE_DIRS}
        )

target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${GLFW_LIBRARIES} ${PNG_LIBRARIES} ${OPENGL_LIBRARIES} glbinding::glbinding fmt-header-only spdlog glm)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DSPDLOG_FMT_EXTERNAL)

add_dependencies(${PROJECT_NAME} shaders-target textures-target)

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
