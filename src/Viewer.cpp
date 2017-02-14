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

Viewer::Viewer() : width(0), height(0), last_fps_time(glfwGetTime()), framecount(0), fps(0), running(false),
                   guitimer(0.0f), trajectory({{{0, 0, 0, 0}, {32, 0, 0, 0}, {0, 32, 0, 0}, {0, 0, 32, 0}}}) {

    last_time = glfwGetTime();

    particleProgram.compileShader(GL_VERTEX_SHADER, "shaders/particles/vertex.glsl");
    particleProgram.compileShader(GL_FRAGMENT_SHADER, "shaders/particles/fragment.glsl");
    particleProgram.link();

    glGenBuffers(2, buffers);

    // camera
    {
        camera.setPosition(glm::vec3(20, 10, 10));
        camera.rotate(30, 240);
        glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(transformation_buffer_t), NULL, GL_DYNAMIC_DRAW);
    }
    // light
    {
        lightparams_t params;
        params.lightpos = glm::vec3(-20, 80, -2);
        params.spotdir = glm::normalize(glm::vec3(.25, -1, .25));
        params.spotexponent = 8;
        params.lightintensity = 10000;
        glBindBuffer(GL_UNIFORM_BUFFER, lightingBuffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(lightparams), &params, GL_STATIC_DRAW);
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, transformationBuffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightingBuffer);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClearDepth(1.0f);

    trajectory.show(0);
    pointSprite.setPositionBuffer(trajectory.getPositionBuffer(), 4*sizeof(float), 0);

    updateViewMatrix();

}

Viewer::~Viewer() {
    glDeleteBuffers(2, &buffers[0]);
}

void Viewer::updateViewMatrix() {
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof (transformation_buffer_t, viewmat),
                    sizeof(glm::mat4), glm::value_ptr(camera.getViewMatrix()));
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof (transformation_buffer_t, invviewmat),
                    sizeof(glm::mat4), glm::value_ptr(glm::inverse(camera.getViewMatrix())));

    glm::vec3 eyepos = camera.position();
    glBindBuffer(GL_UNIFORM_BUFFER, lightingBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof (lightparams_t, eyepos), sizeof(eyepos), glm::value_ptr(eyepos));
}

void Viewer::onMouseMove(double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
            camera.zoom(static_cast<float>(x + y));
        } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
            camera.movex(static_cast<float>(x));
            camera.movey(static_cast<float>(y));
        } else {
            camera.rotate(static_cast<float>(y), static_cast<float>(-x));
        }
        updateViewMatrix();
    }
}

void Viewer::onMouseDown(int button) {

}

void Viewer::onMouseUp(int button) {

}

void Viewer::onKeyUp(int key) {

}

void Viewer::onKeyDown(int key) {

}

void Viewer::resize(unsigned int width, unsigned int height) {
    Viewer::width = width;
    Viewer::height = height;
    projmat = glm::perspective(45.f * float(M_PI / 180.), float(width) / float(height), 1.0f, 200.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof(transformation_buffer_t, projmat), sizeof(glm::mat4),
                    glm::value_ptr(projmat));
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

    if(running) {
        trajectory.show(0);
    }

    particleProgram.use();

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
    return true;
}

}

