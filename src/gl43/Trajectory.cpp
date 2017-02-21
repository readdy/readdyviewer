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
    glm::vec3 max, min;
    {
        max = min = {entries[0][0].pos.x, entries[0][0].pos.y, entries[0][0].pos.z};
        for (const auto &frame : entries) {
            currentNParticles.push_back(frame.size());
            for (const auto &entry : frame) {
                if (entry.pos.x < min.x) min.x = entry.pos.x;
                if (entry.pos.y < min.y) min.y = entry.pos.y;
                if (entry.pos.z < min.z) min.z = entry.pos.z;

                if (entry.pos.x > max.x) max.x = entry.pos.x;
                if (entry.pos.y > max.y) max.y = entry.pos.y;
                if (entry.pos.z > max.z) max.z = entry.pos.z;
                if (entry.type > maxType) maxType = entry.type;
            }
        }
    }
    // translate s.t. min is at bbox_min
    glm::vec3 posTranslation = bbox_min - min;
    // scale s.t. max is inside (10, 10, 10)
    float scale;
    if(max.x > max.y && max.x > max.z) {
        // x largest
        scale = 11.f / (max.x + posTranslation.x);
    } else if(max.y > max.x && max.y > max.z) {
        // y largest
        scale = 11.f / (max.y + posTranslation.y);
    } else {
        // z largest
        scale = 11.f / (max.z + posTranslation.z);
    }

    maxNParticles = 0;
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        maxNParticles = std::max(maxNParticles, it->size());
    }

    posTypes.resize(maxNParticles * T);
    {
        for (std::size_t i = 0; i < entries.size(); ++i) {
            const auto &frame = entries.at(i);
            std::size_t j = 0;
            auto it_pt = posTypes.begin();
            auto it_frame = frame.begin();
            for (; it_pt != posTypes.end(); ++it_pt, ++j, ++it_frame) {
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
            conf.push_back(std::move(c));
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
    if (++t <= T) {
        GLuint tmpBuffer;
        glGenBuffers(1, &tmpBuffer);
        glBindBuffer(GL_COPY_READ_BUFFER, tmpBuffer);
        glBufferData(GL_COPY_READ_BUFFER, 4 * sizeof(float) * getCurrentNParticles(), posTypes.data(), GL_STREAM_COPY);
        glBindBuffer(GL_COPY_WRITE_BUFFER, positionBuffer);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 4 * sizeof(float) * getCurrentNParticles());
        glDeleteBuffers(1, &tmpBuffer);
    }
}

std::size_t Trajectory::nTimeSteps() const {
    return T;
}

std::size_t Trajectory::currentTimeStep() const {
    return t;
}

std::size_t Trajectory::getCurrentNParticles() const {
    return currentNParticles.at(t-1);
}

void Trajectory::reset() {
    t = 1;
}

GLuint Trajectory::getConfigurationBuffer() const {
    return particleConfigurationBuffer;
}

TrajectoryEntry::TrajectoryEntry(float x, float y, float z, TrajectoryEntry::type_t type, unsigned long id)
        : pos(x,y,z), type(type), id(id){

}

TrajectoryEntry::TrajectoryEntry() = default;
}
