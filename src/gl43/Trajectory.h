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

#include "common43.h"

namespace rv {

struct TrajectoryEntry {
    using pos_t = glm::vec3;
    using type_t = unsigned int;
    pos_t pos;
    type_t type;
    unsigned long id;
};

struct TrajectoryConfiguration {
    std::unordered_map<TrajectoryEntry::type_t, glm::vec3> colors {};
    std::unordered_map<TrajectoryEntry::type_t, float> radii {};
};

class Trajectory {
public:
    Trajectory(const std::vector<std::vector<TrajectoryEntry>> &entries, const TrajectoryConfiguration &config);
    ~Trajectory();

    GLuint getPositionBuffer() const;
    GLuint getConfigurationBuffer() const;

    void frame();
    void reset();
    std::size_t nTimeSteps() const;
    std::size_t currentTimeStep() const;

    std::size_t getCurrentNParticles() const;

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
        };
        GLuint buffers[2];
    };
    std::size_t maxNParticles;
    std::vector<glm::vec4> posTypes;
    std::vector<std::size_t> currentNParticles;
    std::size_t T;
    TrajectoryConfiguration config;
    TrajectoryEntry::type_t maxType;
    glm::vec3 defaultColor;
    float defaultRadius;
};
}


#endif //PROJECT_TRAJECTORY_H
