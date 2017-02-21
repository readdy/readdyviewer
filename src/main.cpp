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
    Viewer *viewer = reinterpret_cast<Viewer *>(glfwGetWindowUserPointer(window));
    viewer->onMouseMove(x - cursor.x, y - cursor.y);
    cursor.x = x;
    cursor.y = y;
}

void onKeyEvent(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
    Viewer *viewer = reinterpret_cast<Viewer *>(glfwGetWindowUserPointer(window));
    if (action == GLFW_RELEASE) {
        viewer->onKeyUp(key);
    } else if (action == GLFW_PRESS) {
        viewer->onKeyDown(key);
    }
}

TrajectoryConfiguration getTrajectoryTestConfig() {
    TrajectoryConfiguration config;
    config.colors[1] = glm::vec3(1, .25, .25);
    config.radii[0] = .1;
    return config;
}

std::vector<std::vector<TrajectoryEntry>> generateTestData() {
    std::vector<std::vector<TrajectoryEntry>> entries;
    auto nFrames = rnd::uniform_int(100, 300);
    entries.reserve((unsigned long) nFrames);
    for (auto i = 0; i < nFrames; ++i) {
        auto nParticles = i + 5;
        std::vector<TrajectoryEntry> frame;
        frame.reserve((unsigned long) nParticles);
        for (auto k = 0; k < nParticles; ++k) {
            TrajectoryEntry e;
            e.pos = glm::vec3(rnd::uniform_real(-10.f, 10.f), rnd::uniform_real(-10.f, 10.f),
                              rnd::uniform_real(-10.f, 10.f));
            e.type = k % 2 == 0 ? 0 : 1;
            e.id = (unsigned long) k;
            frame.push_back(e);
        }
        entries.push_back(std::move(frame));
    }
    return entries;
}

void initialize(bool debugContext, const std::vector<std::vector<TrajectoryEntry>> &data,
                const TrajectoryConfiguration &config) {
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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext ? GL_TRUE.m_value : GL_FALSE.m_value);

    window = glfwCreateWindow(1280, 720, "ReaDDy viewer", NULL, NULL);
    if (window == NULL) {
        throw std::runtime_error("Cannot open window.");
    }
    glfwMakeContextCurrent(window);

    if (debugContext) {
        glDebugMessageCallback(glDebugCallback, NULL);
        glEnable(GL_DEBUG_OUTPUT);
    }


    viewer = std::make_unique<Viewer>(data, config);
    GL_CHECK_ERROR()
    glfwSetWindowUserPointer(window, viewer.get());
    glfwGetCursorPos(window, &cursor.x, &cursor.y);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetKeyCallback(window, onKeyEvent);

    GL_CHECK_ERROR()

    {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        viewer->resize(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
        {
            GL_CHECK_ERROR()
        }
    }

    while (!glfwWindowShouldClose(window)) {

        if (!viewer->frame()) {
            break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void cleanup() {
    if (viewer) {
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
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

PYBIND11_PLUGIN(readdyviewer) {
    py::module m("readdyviewer");

    py::class_<rv::TrajectoryConfiguration>(m, "Configuration")
            .def(py::init<>())
            .def_readwrite("colors", &rv::TrajectoryConfiguration::colors)
            .def_readwrite("radii", &rv::TrajectoryConfiguration::radii);

    py::class_<rv::TrajectoryEntry>(m, "TrajectoryEntry")
            .def(py::init < float, float, float, unsigned int, unsigned long > ());

    m.def("watch_npy", [](
            py::array_t<float, py::array::c_style | py::array::forcecast> &positions,
            py::array_t<unsigned int, py::array::c_style | py::array::forcecast> &types,
            py::array_t<unsigned long, py::array::c_style | py::array::forcecast> &ids,
            py::array_t<unsigned int, py::array::c_style | py::array::forcecast> &n_particles_arr,
            const rv::TrajectoryConfiguration &config) {
        auto info_positions = positions.request(false);
        auto info_types = types.request(false);
        auto info_ids = ids.request(false);
        auto info_n_particles = n_particles_arr.request(false);
        const auto n_frames = info_positions.shape[0];
        const auto max_n_particles = info_positions.shape[1];
        rv::log::debug("got n frames = {} with max_n_particles = {}", n_frames, max_n_particles);
        if (n_frames == info_types.shape[0] && n_frames == info_ids.shape[0] && n_frames == info_n_particles.shape[0]) {
            if (max_n_particles == info_types.shape[1] && max_n_particles == info_ids.shape[1]) {
                if (info_positions.shape[2] == 3) {
                    auto data_positions = (float(*)[n_frames][max_n_particles]) positions.data(0);
                    auto data_types = (unsigned int(*)[n_frames]) types.data(0);
                    auto data_ids = (unsigned long(*)[n_frames]) ids.data(0);
                    auto data_n_particles = (unsigned int(*)) n_particles_arr.data(0);

                    std::vector<std::vector<rv::TrajectoryEntry>> data;
                    data.reserve(n_frames);
                    for(std::size_t t = 0; t < n_frames; ++t) {
                        rv::log::debug("converting frame {} ... ", t);
                        std::vector<rv::TrajectoryEntry> frame;

                        auto n_particles = data_n_particles[t];
                        rv::log::debug("\t\t got n_particles={}", n_particles);
                        frame.reserve(n_particles);
                        for(std::size_t p = 0; p < n_particles; ++p) {
                            frame.emplace_back(data_positions[t][p][0], data_positions[t][p][1], data_positions[t][p][2], data_types[t][p], data_ids[t][p]);
                        }

                        data.push_back(std::move(frame));
                        rv::log::debug(" ... done");
                    }

                    try {
                        rv::initialize(false, data, config);
                        rv::cleanup();
                    } catch (const std::exception &e) {
                        rv::log::error("Encountered exception: {}", e.what());
                        rv::cleanup();
                        return -1;
                    }
                    return 0;
                } else {
                    rv::log::error("The third dimension of positions should be of shape 3!");
                }
            } else {
                rv::log::error(
                        "The shapes did not match!! (max_n_particles = {}, typeslen = {}, idslen = {}",
                        max_n_particles, info_types.shape[1], info_ids.shape[1]);
            }
        } else {
            rv::log::error(
                    "The shapes did not match!! (n_frames = {}, types.shape[0]={}, ids.shape[0]={}, nparts.shape[0]={}",
                    n_frames, info_types.shape[0], info_ids.shape[0], info_n_particles.shape[0]);
        }
        return -1;
    });
    m.def("watch",
          [](const std::vector<std::vector<rv::TrajectoryEntry>> &data, const rv::TrajectoryConfiguration &config) {
              try {
                  rv::initialize(false, data, config);
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
        rv::initialize(false, rv::generateTestData(), rv::getTrajectoryTestConfig());
        rv::cleanup();
    } catch (const std::exception &e) {
        rv::log::error("Encountered exception: {}", e.what());
        rv::cleanup();
        return -1;
    }
    return 0;
}
#endif

