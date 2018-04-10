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
 * @file Trajectory.h
 * @brief << brief description >>
 * @author clonker
 * @date 13.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#ifndef PROJECT_TRAJECTORY_H
#define PROJECT_TRAJECTORY_H

#include "common.h"
#include "preprocessing.h"

namespace rv {

struct TrajectoryEntry {
    using pos_t = glm::vec3;
    using type_t = unsigned int;

    TrajectoryEntry();

    TrajectoryEntry(float x, float y, float z, type_t type, unsigned long id);

    pos_t pos;
    type_t type;
    unsigned long id;
};

struct TrajectoryConfiguration {
    std::unordered_map<TrajectoryEntry::type_t, glm::vec3> colors{};
    std::unordered_map<TrajectoryEntry::type_t, float> radii{};
    unsigned int stride = 1;
    unsigned int wait = 1;
    glm::vec3 clearcolor;
    std::size_t smoothing = 1;
    float bondRadius = .1f;
    float smoothingCutoff = 2.f;
    std::string resourcedir;
    glm::vec3 edgecolor = glm::vec3(.5, .5, .5);
    bool drawPeriodic = false;
    glm::vec3 boxSize = glm::vec3(0, 0, 0);
};

class Trajectory {
public:
    Trajectory(TrajectoryEntries entries, const TrajectoryConfiguration &config);

    ~Trajectory();

    GLuint getPositionBuffer() const;

    GLuint getConfigurationBuffer() const;

    GLuint getEdgeBufferFrom() const;

    GLuint getEdgeBufferTo() const;

    GLuint getEdgeColorBuffer() const;

    void frame(bool updateTime = true);

    void reset();

    std::size_t nTimeSteps() const;

    std::size_t currentTimeStep() const;

    std::size_t getCurrentNParticles() const;

    std::size_t getCurrentNEdges() const;

    const glm::vec3 &max() const;

    const glm::vec3 &min() const;

private:
    std::size_t t;
    using particle_config_t = struct particle_config {
        glm::vec4 color;
        float radius;
        float padding[3];
    };
    union {
        struct {
            GLuint positionBuffer;
            GLuint particleConfigurationBuffer;
            GLuint edgeBufferFrom;
            GLuint edgeBufferTo;
            GLuint edgeColorBuffer;
        };
        GLuint buffers[5];
    };
    std::size_t maxNEdges;
    std::vector<glm::vec4> edgePositionsFrom;
    std::vector<glm::vec4> edgePositionsTo;
    std::vector<glm::vec4> edgeColors;
    std::vector<std::size_t> currentNEdges;
    TrajectoryConfiguration config;
    glm::vec3 defaultColor;
    float defaultRadius;
    glm::vec3 _max, _min;

    TrajectoryEntries entries;

    void setUpEdges();

    void setUpConfig(const TrajectoryConfiguration &config) const;

    void updateParticlePositions() const;

    void updateEdgesFrom() const;

    void updateEdgesTo() const;

    void updateEdgeColors() const;

    std::size_t maxNParticles() const;
};
}


#endif //PROJECT_TRAJECTORY_H
