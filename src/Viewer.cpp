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


/**
 *
 *
 * @file Viewer.cpp
 * @brief 
 * @author clonker
 * @date 2/12/17
 */
#include "Viewer.h"
#include <sstream>

namespace rv {

std::vector<Light> getLights() {
    std::vector<Light> lights;
    lights.emplace_back(glm::vec3(20, 20, 20), glm::normalize(glm::vec3(-1, -1, -1)), glm::vec3(.5, .5, .5), .8f, LightType::AMBIENT);
    lights.emplace_back(glm::vec3(40, 40, 40), glm::normalize(glm::vec3(1, 1, 1)), glm::vec3(.5, .5, .5), .4f, LightType::DIRECTION);
    return lights;
}

Viewer::Viewer(const std::vector<std::vector<TrajectoryEntry>> &entries)
        : width(0), height(0), last_fps_time(glfwGetTime()), framecount(0), fps(0), running(false),
          guitimer(0.0f), trajectory(entries),
          interrupt(false), lights(getLights()) {
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "OpenGL error detected at constructor: 0x" << std::hex << err << std::endl;
            log::error(ss.str());
        }
    }

    last_time = glfwGetTime();

    particleProgram.compileShader(GL_VERTEX_SHADER, "shaders/particles/vertex.glsl");
    particleProgram.compileShader(GL_FRAGMENT_SHADER, "shaders/particles/fragment.glsl");
    particleProgram.compileShader(GL_FRAGMENT_SHADER, "shaders/light/light.glsl");
    particleProgram.link();

    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "OpenGL error detected at constructor2: 0x" << std::hex << err << std::endl;
            log::error(ss.str());
        }
    }

    glGenBuffers(sizeof(buffers)/sizeof(buffers[0]), buffers);

    // camera
    {
        camera.setPosition(glm::vec3(20, 20, 20));
        camera.rotate(30, 240);
        glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(transformation_buffer_t), NULL, GL_DYNAMIC_DRAW);
    }
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "OpenGL error detected at constructor3: 0x" << std::hex << err << std::endl;
            log::error(ss.str());
        }
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, transformationBuffer);
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "OpenGL error detected at constructor4: 0x" << std::hex << err << std::endl;
            log::error(ss.str());
        }
    }
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lights.getLightsBuffer());
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "OpenGL error detected at constructor5: 0x" << std::hex << err << std::endl;
            log::error(ss.str());
        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    trajectory.frame();
    pointSprite.setPositionBuffer(trajectory.getPositionBuffer(), 4 * sizeof(float), 0);

    updateViewMatrix();

    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "OpenGL error detected at constructor6: 0x" << std::hex << err << std::endl;
            log::error(ss.str());
        }
    }

}

Viewer::~Viewer() {
    glDeleteBuffers(sizeof(buffers)/sizeof(buffers[0]), &buffers[0]);
}

void Viewer::updateViewMatrix() {
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "OpenGL error detected at update view matrix: 0x" << std::hex << err << std::endl;
            log::error(ss.str());
        }
    }
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof (transformation_buffer_t, viewmat),
                    sizeof(glm::mat4), glm::value_ptr(camera.getViewMatrix()));
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof (transformation_buffer_t, invviewmat),
                    sizeof(glm::mat4), glm::value_ptr(glm::inverse(camera.getViewMatrix())));
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "OpenGL error detected after update view matrix: 0x" << std::hex << err << std::endl;
            log::error(ss.str());
        }
    }
}

void Viewer::onMouseMove(double x, double y) {
    static float dampening = .1;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
            camera.zoom(static_cast<float>(x + y));
        } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
            camera.movex(static_cast<float>(x));
            camera.movey(static_cast<float>(y));
        } else {
            camera.rotate(dampening * static_cast<float>(y), dampening * static_cast<float>(-x));
        }
        updateViewMatrix();
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
        camera.zoom(static_cast<float>(x + y));
    }
}

void Viewer::onMouseDown(int button) {

}

void Viewer::onMouseUp(int button) {

}

void Viewer::onKeyUp(int key) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
            interrupt = true;
            break;
        default:
            log::trace("key {} was not recognized", key);
            break;
    }
}

void Viewer::onKeyDown(int key) {
    switch (key) {
        case GLFW_KEY_SPACE:
            running = !running;
            break;
        default:
            log::trace("key {} was not recognized", key);
            break;
    }
}

void Viewer::resize(unsigned int width, unsigned int height) {
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "OpenGL error detected at resize: 0x" << std::hex << err << std::endl;
            log::error(ss.str());
        }
    }
    Viewer::width = width;
    Viewer::height = height;
//    projmat = glm::perspective(45.f * float(M_PI / 180.), float(width) / float(height), 1.0f, 200.0f);
    projmat = glm::infinitePerspective(45.f * float(M_PI / 180.), float(width) / float(height), 1.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof(transformation_buffer_t, projmat), sizeof(glm::mat4),
                    glm::value_ptr(projmat));
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "OpenGL error detected after resize: 0x" << std::hex << err << std::endl;
            log::error(ss.str());
        }
    }
}

bool Viewer::frame() {
    last_time += glfwGetTime() - last_time;
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::stringstream ss;
        ss << "OpenGL error detected at frame begin: 0x" << std::hex << err << std::endl;
        log::error(ss.str());
        return false;
    }
    // specify the viewport size
    glViewport(0, 0, width, height);
    // clear the color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (running) {
        if (trajectory.currentTimeStep() < trajectory.nTimeSteps()) {
            trajectory.frame();
        } else {
            running = false;
        }
    }
    GL_CHECK_ERROR()
    particleProgram.use();
    GL_CHECK_ERROR()
    pointSprite.setPositionBuffer(trajectory.getPositionBuffer());
    GL_CHECK_ERROR()
    pointSprite.render(trajectory.getCurrentNParticles());
    GL_CHECK_ERROR()

    GL_CHECK_ERROR()
    framing.render();
    GL_CHECK_ERROR()

    // determine the framerate every second
    framecount++;
    if (glfwGetTime() >= last_fps_time + 1) {
        fps = framecount;
        framecount = 0;
        last_fps_time = glfwGetTime();
    }
    {
        log::trace("FPS: {}", fps);
    }
    return !interrupt;
}

}

