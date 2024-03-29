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

namespace rv {

std::vector<Light> getLights(float intensity) {
    std::vector<Light> lights;
    lights.emplace_back(glm::vec3(20, 20, 20), glm::normalize(glm::vec3(-1, -1, -1)), glm::vec3(.5, .5, .5), 2.f*intensity, LightType::AMBIENT);
    lights.emplace_back(glm::vec3(40, 40, 40), glm::normalize(glm::vec3(1, 1, 1)), glm::vec3(.5, .5, .5), 1.f*intensity, LightType::HEAD);
    return lights;
}

Viewer::Viewer(rv::TrajectoryEntries entries, const std::vector<Light> &lights, const TrajectoryConfiguration& config)
        : width(0), height(0), last_fps_time(glfwGetTime()), framecount(0), fps(0), running(false),
          guitimer(0.0f), trajectory(std::move(entries), config), drawPeriodic(config.drawPeriodic),
          interrupt(false), lightArrangement(lights.empty() ? getLights(config.intensity) : lights), framing(config.resourcedir),
          pointSprite(config.drawPeriodic, config.boxSize), wait(config.wait), currentWait(0) {
    GL_CHECK_ERROR()

    last_time = glfwGetTime();

    particleProgram.pathPrefix() = config.resourcedir;
    particleProgram.compileShader(GL_VERTEX_SHADER, "shaders/particles/vertex.glsl");
    particleProgram.compileShader(GL_FRAGMENT_SHADER, {"shaders/particles/fragment.glsl", "shaders/light/light.glsl"});
    particleProgram.link();

    GL_CHECK_ERROR()

    edgeProgram.pathPrefix() = config.resourcedir;
    edgeProgram.compileShader(GL_VERTEX_SHADER, "shaders/edge/vertex.glsl",
                              fmt::format(
                                      "const vec3 edgeColor = vec3({}, {}, {});\nconst float radius = {};\n",
                                          config.edgecolor.r, config.edgecolor.g, config.edgecolor.b, config.bondRadius));
    edgeProgram.compileShader(GL_FRAGMENT_SHADER, {"shaders/edge/fragment.glsl", "shaders/light/light.glsl"},
                              fmt::format("const float cutoff = {};\n", config.smoothingCutoff));
    edgeProgram.link();

    GL_CHECK_ERROR()

    glGenBuffers(sizeof(buffers)/sizeof(buffers[0]), buffers);

    // camera
    {
        camera.setPosition(trajectory.max() + glm::vec3(50, 50, 50));
        camera.rotate(30, 240);
        glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(transformation_buffer_t), nullptr, GL_DYNAMIC_DRAW);
    }
    GL_CHECK_ERROR()
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, transformationBuffer);
    GL_CHECK_ERROR()
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, trajectory.getConfigurationBuffer());
    GL_CHECK_ERROR()
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightArrangement.getLightsBuffer());
    GL_CHECK_ERROR()

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CCW);

    const auto &clearcolor = config.clearcolor;
    glClearColor(clearcolor.r, clearcolor.g, clearcolor.b, 1.0f);
    glClearDepth(1.0f);

    trajectory.frame();
    pointSprite.setPositionBuffer(trajectory.getPositionBuffer(), 4 * sizeof(float), 0);
    edgeSprite.setEdgesFromBuffer(trajectory.getEdgeBufferFrom(), 4*sizeof(float), 0);
    edgeSprite.setEdgesToBuffer(trajectory.getEdgeBufferTo(), 4*sizeof(float), 0);
    edgeSprite.setEdgeColorBuffer(trajectory.getEdgeColorBuffer(), 4*sizeof(float), 0);
    edgeSprite.render(static_cast<GLuint>(trajectory.getCurrentNEdges()));

    updateViewMatrix();

    GL_CHECK_ERROR()

}

Viewer::~Viewer() {
    glDeleteBuffers(sizeof(buffers)/sizeof(buffers[0]), &buffers[0]);
}

void Viewer::updateViewMatrix() {
    GL_CHECK_ERROR()
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof (transformation_buffer_t, viewmat),
                    sizeof(glm::mat4), glm::value_ptr(camera.getViewMatrix()));
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof (transformation_buffer_t, invviewmat),
                    sizeof(glm::mat4), glm::value_ptr(glm::inverse(camera.getViewMatrix())));
    GL_CHECK_ERROR()
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
        case GLFW_KEY_TAB:
            trajectory.reset();
            break;
        default:
            log::trace("key {} was not recognized", key);
            break;
    }
}

void Viewer::resize(unsigned int width, unsigned int height) {
    GL_CHECK_ERROR()
    Viewer::width = width;
    Viewer::height = height;
    projmat = glm::infinitePerspective(45.f * float(M_PI / 180.), float(width) / float(height), 1.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof(transformation_buffer_t, projmat), sizeof(glm::mat4),
                    glm::value_ptr(projmat));
    GL_CHECK_ERROR()
}

bool Viewer::frame() {
    last_time += glfwGetTime() - last_time;
    GL_CHECK_ERROR()
    // specify the viewport size
    glViewport(0, 0, width, height);
    // clear the color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (running) {
        if (trajectory.currentTimeStep() < trajectory.nTimeSteps()) {
            if(currentWait == wait - 1) {
                trajectory.frame(true);
                currentWait = 0;
            } else {
                trajectory.frame(false);
                ++currentWait;
            }
        } else {
            running = false;
        }
    }
    GL_CHECK_ERROR()
    particleProgram.use();
    GL_CHECK_ERROR()
    pointSprite.setPositionBuffer(trajectory.getPositionBuffer());
    GL_CHECK_ERROR()
    pointSprite.render(static_cast<GLuint>(trajectory.getCurrentNParticles()));

    GL_CHECK_ERROR()
    if(trajectory.getCurrentNEdges() > 0) {
        edgeProgram.use();
        GL_CHECK_ERROR()
        edgeSprite.setEdgesFromBuffer(trajectory.getEdgeBufferFrom());
        edgeSprite.setEdgesToBuffer(trajectory.getEdgeBufferTo());
        edgeSprite.setEdgeColorBuffer(trajectory.getEdgeColorBuffer());
        GL_CHECK_ERROR()
        edgeSprite.render(static_cast<GLuint>(trajectory.getCurrentNEdges()));
        GL_CHECK_ERROR()
    }
    // framing.render();
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

