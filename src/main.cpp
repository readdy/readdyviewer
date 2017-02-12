/********************************************************************
 * Copyright © 2016 Computational Molecular Biology Group,          *
 *                  Freie Universität Berlin (GER)                  *
 *                                                                  *
 * This file is part of ReaDDy.                                     *
 *                                                                  *
 * ReaDDy is free software: you can redistribute it and/or modify   *
 * it under the terms of the GNU Lesser General Public License as   *
 * published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU Lesser General Public License for more details.              *
 *                                                                  *
 * You should have received a copy of the GNU Lesser General        *
 * Public License along with this program. If not, see              *
 * <http://www.gnu.org/licenses/>.                                  *
 ********************************************************************/

#include <glbinding/Binding.h>
#include "common.h"

namespace rv {

GLFWwindow *window = NULL;

void glfwErrorCallback(int error, const char *msg) {
    rv::log::error("GLFW error {}: {}", error, msg);
}

void glDebugCallback(gl::GLenum source, gl::GLenum type, gl::GLuint id, gl::GLenum severity, gl::GLsizei length,
                     const gl::GLchar *message,
                     const void *userParam) {
    log::debug("OpenGL debug message: {}", std::string(message, static_cast<std::size_t>(length)));
}

void initialize(bool debugContext) {
    glbinding::Binding::initialize();
    glbinding::Binding::useCurrentContext();
    if (!glfwInit()) {
        throw std::runtime_error("Could not initialize GLFW!");
    }
    glfwSetErrorCallback(glfwErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext ? GL_TRUE : GL_FALSE);

    window = glfwCreateWindow(1280, 720, "ReaDDy viewer", NULL, NULL);
    if (window == NULL) {
        throw std::runtime_error("Cannot open window.");
    }
    glfwMakeContextCurrent(window);

    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, {"glGetError"});
    glbinding::setAfterCallback([](const glbinding::FunctionCall &) {
        const auto err = glGetError();
        if (err != GL_NO_ERROR) {
            log::error("GL error {}", err);
        }
    });

    if (debugContext) {
        glDebugMessageCallback(glDebugCallback, NULL);
        glEnable(GL_DEBUG_OUTPUT);
    }

    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void cleanup() {
    if (window != NULL) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}


}

int main() {
    try {
        rv::initialize(false);
        rv::cleanup();
    } catch (const std::exception &e) {
        rv::log::error("Encountered exception: {}", e.what());
        rv::cleanup();
        return -1;
    }
    return 0;
}