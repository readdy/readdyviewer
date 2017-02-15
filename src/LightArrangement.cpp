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
 * << detailed description >>
 *
 * @file LightArrangement.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 15.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include <sstream>
#include "LightArrangement.h"

namespace rv {

LightArrangement::LightArrangement(const std::vector<Light> &lights) : lights(lights) {
    glGenBuffers(1, buffers);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, lights.size() * Light::stride(), nullptr, GL_DYNAMIC_COPY);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::stringstream ss;
        ss << "OpenGL error detected at light arrangement alloc: 0x" << std::hex << err << std::endl;
        log::error(ss.str());
    }

    GLuint offset = 0;
    for(const auto& light : lights) {
        light.bindParams(lightsBuffer, offset);
        offset += Light::stride();
    }

    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::stringstream ss;
        ss << "OpenGL error detected after light arrangement alloc: 0x" << std::hex << err << std::endl;
        log::error(ss.str());
    }
}

LightArrangement::~LightArrangement() {
    glDeleteBuffers(1, buffers);
}

GLuint LightArrangement::getLightsBuffer() const {
    return lightsBuffer;
}

const std::vector<Light> &LightArrangement::getLights() const {
    return lights;
}

Light::Light(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &color, float intensity,
             LightType type) {
    params.position = glm::vec4(position, 1);
    params.direction = glm::vec4(direction, 1);
    params.color = glm::vec4(color, 1);
    params.intensity = intensity;
    params.type = static_cast<unsigned int>(type);
}

std::size_t Light::stride() {
    return sizeof(light_params_t);
}

void Light::bindParams(GLuint buffer, GLuint offset) const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + offsetof(light_params_t, position), sizeof(params.position), glm::value_ptr(params.position));
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + offsetof(light_params_t, direction), sizeof(params.direction), glm::value_ptr(params.direction));
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + offsetof(light_params_t, color), sizeof(params.color), glm::value_ptr(params.color));
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + offsetof(light_params_t, intensity), sizeof(params.intensity), &params.intensity);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + offsetof(light_params_t, type), sizeof(params.type), &params.type);
}

Light::~Light() = default;
}