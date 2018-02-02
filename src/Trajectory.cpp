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
 * @file Trajectory.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 13.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include "Trajectory.h"

namespace rv {
Trajectory::Trajectory(const std::vector<std::vector<rv::TrajectoryEntry>> &entries,
                       const TrajectoryConfiguration &config)
        : t(0), config(config), maxType(0), defaultColor(0.25, 0, 1), defaultRadius(.6) {

    T = entries.size();
    currentNParticles.reserve(T);

    glm::vec3 bbox_min{1, 1, 1};
    glm::vec3 bbox_max{50, 50, 50};
    {
        _max = _min = {entries[0][0].pos.x, entries[0][0].pos.y, entries[0][0].pos.z};
        for (const auto &frame : entries) {
            currentNParticles.push_back(frame.size());
            for (const auto &entry : frame) {
                if (entry.pos.x < _min.x) _min.x = entry.pos.x;
                if (entry.pos.y < _min.y) _min.y = entry.pos.y;
                if (entry.pos.z < _min.z) _min.z = entry.pos.z;

                if (entry.pos.x > _max.x) _max.x = entry.pos.x;
                if (entry.pos.y > _max.y) _max.y = entry.pos.y;
                if (entry.pos.z > _max.z) _max.z = entry.pos.z;
                if (entry.type > maxType) maxType = entry.type;
            }
        }
    }
    // translate s.t. min is at bbox_min
    glm::vec3 posTranslation = bbox_min - _min;
    log::debug("translate every particle by {}, {}, {}", posTranslation.x, posTranslation.y, posTranslation.z);
    // scale s.t. max is inside (10, 10, 10)
    float scale;
    if(_max.x > _max.y && _max.x > _max.z) {
        // x largest
        scale = bbox_max.x / (_max.x + posTranslation.x);
    } else if(_max.y > _max.x && _max.y > _max.z) {
        // y largest
        scale = bbox_max.x / (_max.y + posTranslation.y);
    } else {
        // z largest
        scale = bbox_max.x / (_max.z + posTranslation.z);
    }
    scale = 1.0;
    log::debug("scale every particle by {}", scale);

    maxNParticles = 0;
    for (const auto &e : entries) {
        maxNParticles = std::max(maxNParticles, e.size());
    }

    posTypes.resize(maxNParticles * T);
    {
        for (std::size_t i = 0; i < entries.size(); ++i) {
            const auto &frame = entries.at(i);
            std::size_t j = 0;
            std::size_t offset = i * maxNParticles;
            auto it_pt = posTypes.begin() + offset;
            auto it_frame = frame.begin();
            for (; it_pt != posTypes.begin() + offset + frame.size(); ++it_pt, ++j, ++it_frame) {
                // project into [0, 10]**3
                if(j < frame.size()) {
                    *it_pt = glm::vec4(scale * (it_frame->pos + posTranslation), it_frame->type);
                }
            }
        }
    }

    glGenBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
    // positions
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(float) * maxNParticles, nullptr, GL_DYNAMIC_COPY);
    }
    // config
    {
        std::vector<particle_config_t> conf;
        conf.reserve(maxType + 1);
        for(unsigned int i = 0; i < maxType + 1; ++i) {
            particle_config_t c;
            {
                decltype(config.colors.begin()) it;
                if ((it = config.colors.find(i)) != config.colors.end()) {
                    c.color = glm::vec4(it->second, 0);
                } else {
                    c.color = glm::vec4(defaultColor, 0);
                }
            }
            {
                decltype(config.radii.begin()) it;
                if((it = config.radii.find(i)) != config.radii.end()) {
                    c.radius = it->second;
                } else {
                    c.radius = defaultRadius;
                }
            }
            conf.push_back(c);
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleConfigurationBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, conf.size() * sizeof(particle_config_t), nullptr, GL_DYNAMIC_COPY);
        GL_CHECK_ERROR()
        // bind data
        {
            GLuint offset = 0;
            for (const auto &c : conf) {
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + offsetof(particle_config_t, color), sizeof(c.color), glm::value_ptr(c.color));
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + offsetof(particle_config_t, radius), sizeof(c.radius), &c.radius);
                offset += sizeof(particle_config_t);
            }
        }
        GL_CHECK_ERROR()
    }

}

Trajectory::~Trajectory() {
    glDeleteBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
}

GLuint Trajectory::getPositionBuffer() const {
    return positionBuffer;
}

void Trajectory::frame() {
    t += config.stride;
    if (t <= T) {
        log::debug("frame: {} / {}", t, T);
        GLuint tmpBuffer;
        glGenBuffers(1, &tmpBuffer);
        glBindBuffer(GL_COPY_READ_BUFFER, tmpBuffer);
        glBufferData(GL_COPY_READ_BUFFER, 4 * sizeof(float) * getCurrentNParticles(), posTypes.data() + (t-config.stride)*maxNParticles, GL_STREAM_COPY);
        glBindBuffer(GL_COPY_WRITE_BUFFER, positionBuffer);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 4 * sizeof(float) * getCurrentNParticles());
        glDeleteBuffers(1, &tmpBuffer);
    }
}

std::size_t Trajectory::nTimeSteps() const {
    return T;
}

std::size_t Trajectory::currentTimeStep() const {
    return std::min(t, T);
}

std::size_t Trajectory::getCurrentNParticles() const {
    return currentNParticles.at(currentTimeStep()-config.stride);
}

void Trajectory::reset() {
    t = config.stride;
    log::debug("frame: {} / {}", t, T);
    GLuint tmpBuffer;
    glGenBuffers(1, &tmpBuffer);
    glBindBuffer(GL_COPY_READ_BUFFER, tmpBuffer);
    glBufferData(GL_COPY_READ_BUFFER, 4 * sizeof(float) * getCurrentNParticles(), posTypes.data(), GL_STREAM_COPY);
    glBindBuffer(GL_COPY_WRITE_BUFFER, positionBuffer);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 4 * sizeof(float) * getCurrentNParticles());
    glDeleteBuffers(1, &tmpBuffer);
}

GLuint Trajectory::getConfigurationBuffer() const {
    return particleConfigurationBuffer;
}

const glm::vec3 &Trajectory::max() const {
    return _max;
}

const glm::vec3 &Trajectory::min() const {
    return _min;
}

TrajectoryEntry::TrajectoryEntry(float x, float y, float z, TrajectoryEntry::type_t type, unsigned long id)
        : pos(x,y,z), type(type), id(id){

}

TrajectoryEntry::TrajectoryEntry() = default;
}
