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

#include <sstream>
#include "gl43/common43.h"
#include "gl43/Viewer.h"

namespace rv {

GLFWwindow *window = NULL;
std::unique_ptr<Viewer> viewer;
glm::dvec2 cursor;

void glfwErrorCallback(int error, const char *msg) {
    rv::log::error("GLFW error {}: {}", error, msg);
}

void glDebugCallback(gl::GLenum source, gl::GLenum type, gl::GLuint id, gl::GLenum severity, gl::GLsizei length,
                     const gl::GLchar *message,
                     const void *userParam) {
    log::debug("OpenGL debug message: {}", std::string(message, static_cast<std::size_t>(length)));
}

void onMouseMove(GLFWwindow *window, double x, double y) {
    Viewer* viewer = reinterpret_cast<Viewer*>(glfwGetWindowUserPointer(window));
    viewer->onMouseMove(x - cursor.x, y - cursor.y);
    cursor.x = x;
    cursor.y = y;
}
void onKeyEvent (GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
    Viewer* viewer = reinterpret_cast<Viewer*>(glfwGetWindowUserPointer(window));
    if(action == GLFW_RELEASE) {
        viewer->onKeyUp(key);
    } else if(action == GLFW_PRESS) {
        viewer->onKeyDown(key);
    }
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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE.m_value);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext ? GL_TRUE.m_value: GL_FALSE.m_value);

    window = glfwCreateWindow(1280, 720, "ReaDDy viewer", NULL, NULL);
    if (window == NULL) {
        throw std::runtime_error("Cannot open window.");
    }
    glfwMakeContextCurrent(window);

    if (debugContext) {
        glDebugMessageCallback(glDebugCallback, NULL);
        glEnable(GL_DEBUG_OUTPUT);
    }


    {
        //         {{{-10, 0, 0, 0}, {20, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, -10, 0}}};

        std::vector<std::vector<TrajectoryEntry>> entries;
        auto nFrames = rnd::uniform_int(300, 500);
        entries.reserve((unsigned long) nFrames);
        for(auto i = 0; i < nFrames; ++i) {
            auto nParticles = i+5;
            std::vector<TrajectoryEntry> frame;
            frame.reserve((unsigned long) nParticles);
            for(auto k = 0; k < nParticles; ++k) {
                TrajectoryEntry e;
                e.pos = glm::vec3(rnd::uniform_real(-10.f, 10.f), rnd::uniform_real(-10.f, 10.f), rnd::uniform_real(-10.f, 10.f));
                e.type = 0;
                e.id = (unsigned long) k;
                frame.push_back(e);
            }
            entries.push_back(std::move(frame));
        }
        viewer = std::make_unique<Viewer>(entries);
        GL_CHECK_ERROR()
    }
    glfwSetWindowUserPointer(window, viewer.get());
    glfwGetCursorPos(window, &cursor.x, &cursor.y);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetKeyCallback(window, onKeyEvent);

    GL_CHECK_ERROR()

    {
        int w,h;
        glfwGetFramebufferSize(window, &w, &h);
        viewer->resize(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
        {
            GL_CHECK_ERROR()
        }
    }

    while (!glfwWindowShouldClose(window)) {

        if(!viewer->frame()) {
            break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void cleanup() {
    if(viewer) {
        viewer.reset();
    }
    if (window != NULL) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}


}

#if PYLIB
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

PYBIND11_PLUGIN(readdyviewer) {
    pybind11::module m("readdyviewer");

    m.def("watch", []() {
        try {
            rv::initialize(false);
            rv::cleanup();
        } catch (const std::exception &e) {
            rv::log::error("Encountered exception: {}", e.what());
            rv::cleanup();
            return -1;
        }
        return 0;
    });

    return m.ptr();
}
#else
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
#endif

