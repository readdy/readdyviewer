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

#include <cstddef>
#include "Trajectory.h"

namespace rv {
Trajectory::Trajectory(TrajectoryEntries entries, const TrajectoryConfiguration &config)
        : t(0), config(config), entries(std::move(entries)), defaultColor(0.25, 0, 1), defaultRadius(.6) {

    //setUpParticles(entries, edges);

    glGenBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
    // positions
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(float) * maxNParticles(), nullptr, GL_DYNAMIC_COPY);
    }

    setUpEdges();
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, edgeBufferFrom);
        GL_CHECK_ERROR()
        glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(float) * maxNEdges, nullptr, GL_DYNAMIC_COPY);
        GL_CHECK_ERROR()
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, edgeBufferTo);
        GL_CHECK_ERROR()
        glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(float) * maxNEdges, nullptr, GL_DYNAMIC_COPY);
        GL_CHECK_ERROR()
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, edgeColorBuffer);
        GL_CHECK_ERROR()
        glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(float) * maxNEdges, nullptr, GL_DYNAMIC_COPY);
        GL_CHECK_ERROR()
    }

    // config
    setUpConfig(config);

}

void Trajectory::setUpConfig(const TrajectoryConfiguration &config) const {
    std::vector<particle_config_t> conf;
    conf.reserve(entries.maxType + 1);
    for (unsigned int i = 0; i < entries.maxType + 1; ++i) {
        particle_config_t c;
        {
            auto it = config.colors.find(i);
            if (it != config.colors.end()) {
                c.color = glm::vec4(it->second, 0);
                log::debug("assigning type {} color ({}, {}, {})", i, c.color.r, c.color.g, c.color.b);
            } else {
                log::debug("Could not find color for type {}, using default", i);
                c.color = glm::vec4(defaultColor, 0);
            }
        }
        {
            auto it = config.radii.find(i);
            if (it != config.radii.end()) {
                c.radius = it->second;
                log::debug("assigning type {} radius {}", i, c.radius);
            } else {
                log::debug("Could not find radius for type {}, using default", i);
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
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + offsetof(particle_config_t, color), sizeof(c.color),
                            value_ptr(c.color));
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset + offsetof(particle_config_t, radius), sizeof(c.radius),
                            &c.radius);
            offset += sizeof(particle_config_t);
        }
    }
    GL_CHECK_ERROR()
}

/*void Trajectory::setUpParticles(const std::vector<std::vector<TrajectoryEntry>> &entries, rv::edges_type &edges) {
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
    //posTranslation = {0, 0, 0};
    log::debug("translate every particle by {}, {}, {}", posTranslation.x, posTranslation.y, posTranslation.z);
    // scale s.t. max is inside (10, 10, 10)
    float scale;
    if (_max.x > _max.y && _max.x > _max.z) {
        // x largest
        scale = bbox_max.x / (_max.x + posTranslation.x);
    } else if (_max.y > _max.x && _max.y > _max.z) {
        // y largest
        scale = bbox_max.x / (_max.y + posTranslation.y);
    } else {
        // z largest
        scale = bbox_max.x / (_max.z + posTranslation.z);
    }
    scale = 1.0;
    log::debug("scale every particle by {}", scale);

    maxNParticles = 0;
    {
        auto it = std::max_element(currentNParticles.begin(), currentNParticles.end());
        if(it != currentNParticles.end()) maxNParticles = *it;
    }

    for (const auto &e : entries) {
        maxNParticles = std::max(maxNParticles, e.size());
    }

    posTypes.resize(maxNParticles * T);
    const auto smoothing = config.smoothing;
    log::debug("got smoothing window width {}", smoothing);
    if (smoothing == 1) {
        for (std::size_t i = 0; i < entries.size(); ++i) {
            const auto &frame = entries.at(i);
            std::size_t j = 0;
            std::size_t offset = i * maxNParticles;
            auto it_pt = posTypes.begin() + offset;
            auto it_frame = frame.begin();
            for (; it_pt != posTypes.begin() + offset + frame.size(); ++it_pt, ++j, ++it_frame) {
                // project into [0, 10]**3
                if (j < frame.size()) {
                    *it_pt = glm::vec4(scale * (it_frame->pos + posTranslation), it_frame->type);
                }
            }
        }
    } else {
        // smooth
        auto sortedEntries = entries;
        std::size_t t = 0;
        for (auto &frame : sortedEntries) {
            std::vector<std::size_t> indices(frame.size());
            std::iota(indices.begin(), indices.end(), 0);
            std::sort(indices.begin(), indices.end(), [&](const auto i1, const auto i2) {
                return frame.at(i1).id < frame.at(i2).id;
            });

            //reorder(indices.begin(), indices.end(), frame.begin());
            {
                auto idxCopy = indices;
                reorder(frame, idxCopy);
            }

            for (auto it = frame.begin(); it != frame.end() - 1; ++it) {
                if (it->id > (it + 1)->id) {
                    throw std::runtime_error("the frames are not properly sorted by ID, internal error");
                }
            }

            if (edges.size() > t) {
                for (auto &e : edges.at(t)) {
                    std::get<0>(e) = indices.at(std::get<0>(e));
                    std::get<1>(e) = indices.at(std::get<1>(e));
                }
            }

            ++t;
        }
        for (std::size_t i = 0; i < sortedEntries.size(); ++i) {
            std::size_t offset = i * maxNParticles;
            const auto &frame = sortedEntries.at(i);

            std::size_t k = 0;
            for (auto itParticle = frame.begin(); itParticle != frame.end(); ++itParticle, ++k) {

                auto it_pt = posTypes.begin() + offset + k;

                *it_pt = glm::vec4(0, 0, 0, 0);
                std::size_t n = 0;
                for (std::size_t j = i >= smoothing ? i - smoothing : 0;
                     j < std::min(i + smoothing, sortedEntries.size()); ++j) {
                    const auto &otherFrame = sortedEntries.at(j);

                    auto findIt = std::lower_bound(otherFrame.begin(), otherFrame.end(), itParticle->id,
                                                   [](const auto &entry, const auto id) {
                                                       return entry.id < id;
                                                   });
                    if (findIt != otherFrame.end() && findIt->id == itParticle->id) {
                        *it_pt += glm::vec4(scale * (findIt->pos + posTranslation), findIt->type);
                        ++n;
                    }
                }
                *it_pt /= static_cast<double>(n);
            }

        }
    }
}*/

void Trajectory::setUpEdges() {
    const auto &edges = entries.edges;
    const auto T = entries.nParticlesPerFrame.size();
    const auto &posTypes = entries.posTypes;
    const auto maxNParticles = entries.maxNParticles;

    maxNEdges = 0;
    for (const auto &frame : edges) {
        maxNEdges = std::max(maxNEdges, frame.size());
    }
    edgePositionsFrom.resize(maxNEdges * T);
    edgePositionsTo.resize(maxNEdges * T);
    edgeColors.resize(maxNEdges * T);

    for (std::size_t frame = 0; frame < edges.size(); ++frame) {
        const auto &currentEdges = edges.at(frame);
        for (std::size_t edgeIndex = 0; edgeIndex < currentEdges.size(); ++edgeIndex) {
            const auto &edge = currentEdges.at(edgeIndex);
            const auto &p1 = posTypes.at(frame * maxNParticles + std::get<0>(edge));
            const auto &p2 = posTypes.at(frame * maxNParticles + std::get<1>(edge));
            edgePositionsFrom[frame * maxNEdges + edgeIndex] = glm::vec4(p1.x, p1.y, p1.z, 0);
            edgePositionsTo[frame * maxNEdges + edgeIndex] = glm::vec4(p2.x, p2.y, p2.z, 0);
            edgeColors[frame * maxNEdges + edgeIndex] = glm::vec4(1., 0, 0, 0);
        }
        currentNEdges.push_back(currentEdges.size());
    }
}

Trajectory::~Trajectory() {
    glDeleteBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
}

GLuint Trajectory::getPositionBuffer() const {
    return positionBuffer;
}

void Trajectory::frame() {
    if (t < nTimeSteps()) {
        log::debug("frame: {} / {}", t, nTimeSteps() - 1);
        updateParticlePositions();
        updateEdgesFrom();
        updateEdgesTo();
        updateEdgeColors();
    }
    t += 1;
}

void Trajectory::updateEdgeColors() const {
    GLuint tmpBuffer;
    glGenBuffers(1, &tmpBuffer);
    glBindBuffer(GL_COPY_READ_BUFFER, tmpBuffer);
    glBufferData(GL_COPY_READ_BUFFER, 4 * sizeof(float) * getCurrentNEdges(),
                 edgeColors.data() + t * maxNEdges, GL_STREAM_COPY);
    glBindBuffer(GL_COPY_WRITE_BUFFER, edgeColorBuffer);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
                        4 * sizeof(float) * getCurrentNEdges());
    glDeleteBuffers(1, &tmpBuffer);
}

void Trajectory::updateEdgesFrom() const {
    GLuint tmpBuffer;
    glGenBuffers(1, &tmpBuffer);
    glBindBuffer(GL_COPY_READ_BUFFER, tmpBuffer);
    glBufferData(GL_COPY_READ_BUFFER, 4 * sizeof(float) * getCurrentNEdges(),
                 edgePositionsFrom.data() + t * maxNEdges, GL_STREAM_COPY);
    glBindBuffer(GL_COPY_WRITE_BUFFER, edgeBufferFrom);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 4 * sizeof(float) * getCurrentNEdges());
    glDeleteBuffers(1, &tmpBuffer);
}

void Trajectory::updateEdgesTo() const {
    GLuint tmpBuffer;
    glGenBuffers(1, &tmpBuffer);
    glBindBuffer(GL_COPY_READ_BUFFER, tmpBuffer);
    glBufferData(GL_COPY_READ_BUFFER, 4 * sizeof(float) * getCurrentNEdges(),
                 edgePositionsTo.data() + t * maxNEdges, GL_STREAM_COPY);
    glBindBuffer(GL_COPY_WRITE_BUFFER, edgeBufferTo);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 4 * sizeof(float) * getCurrentNEdges());
    glDeleteBuffers(1, &tmpBuffer);
}

void Trajectory::updateParticlePositions() const {
    GLuint tmpBuffer;
    glGenBuffers(1, &tmpBuffer);
    glBindBuffer(GL_COPY_READ_BUFFER, tmpBuffer);
    glBufferData(GL_COPY_READ_BUFFER, 4 * sizeof(float) * getCurrentNParticles(),
                 entries.posTypes.data() + t * entries.maxNParticles, GL_STREAM_COPY);
    glBindBuffer(GL_COPY_WRITE_BUFFER, positionBuffer);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 4 * sizeof(float) * getCurrentNParticles());
    glDeleteBuffers(1, &tmpBuffer);
}

std::size_t Trajectory::nTimeSteps() const {
    return entries.nParticlesPerFrame.size();
}

std::size_t Trajectory::currentTimeStep() const {
    return std::min(t, entries.nParticlesPerFrame.size()-1);
}

std::size_t Trajectory::getCurrentNParticles() const {
    return entries.nParticlesPerFrame.at(currentTimeStep());
}

void Trajectory::reset() {
    t = 0;
    log::debug("frame: {} / {}", t, nTimeSteps() - 1);
    updateParticlePositions();
    updateEdgesFrom();
    updateEdgesTo();
    /*GLuint tmpBuffer;
    glGenBuffers(1, &tmpBuffer);
    glBindBuffer(GL_COPY_READ_BUFFER, tmpBuffer);
    glBufferData(GL_COPY_READ_BUFFER, 4 * sizeof(float) * getCurrentNParticles(), posTypes.data(), GL_STREAM_COPY);
    glBindBuffer(GL_COPY_WRITE_BUFFER, positionBuffer);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 4 * sizeof(float) * getCurrentNParticles());
    glDeleteBuffers(1, &tmpBuffer);*/
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

GLuint Trajectory::getEdgeBufferFrom() const {
    return edgeBufferFrom;
}

GLuint Trajectory::getEdgeColorBuffer() const {
    return edgeColorBuffer;
}

std::size_t Trajectory::getCurrentNEdges() const {
    if (currentNEdges.empty()) return 0;
    return currentNEdges.at(currentTimeStep());
}

GLuint Trajectory::getEdgeBufferTo() const {
    return edgeBufferTo;
}

std::size_t Trajectory::maxNParticles() const {
    return entries.maxNParticles;
}

TrajectoryEntry::TrajectoryEntry(float x, float y, float z, TrajectoryEntry::type_t type, unsigned long id)
        : pos(x, y, z), type(type), id(id) {

}

TrajectoryEntry::TrajectoryEntry() = default;
}
