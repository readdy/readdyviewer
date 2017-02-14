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
Trajectory::Trajectory(const std::vector<std::vector<rv::TrajectoryEntry>> &entries) : t(0), entries(entries){
    maxNParticles = 0;
    for(auto it = entries.begin(); it != entries.end(); ++it) {
        maxNParticles = std::max(maxNParticles, it->size());
    }

    glGenBuffers(2, buffers);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(float) * maxNParticles, NULL, GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, deactivatedBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(bool) * maxNParticles, NULL, GL_DYNAMIC_COPY);


}

Trajectory::~Trajectory() {
    glDeleteBuffers(2, buffers);
}

GLuint Trajectory::getPositionBuffer() const {
    return positionBuffer;
}

GLuint Trajectory::getDeactivatedBuffer() const {
    return deactivatedBuffer;
}

void Trajectory::show(unsigned long step) {
    t = step;
    std::vector<glm::vec4> positionTypes;
    for(const auto& entry : entries[t]) {
        glm::vec4 posType {entry.x, entry.y, entry.z, entry.type};
        positionTypes.push_back(posType);
    }

    {
        GLuint tmpBuffer;
        glGenBuffers(1, &tmpBuffer);

        glBindBuffer(GL_COPY_READ_BUFFER, tmpBuffer);
        glBufferData(GL_COPY_READ_BUFFER, 4 * sizeof(float) * positionTypes.size(), positionTypes.data(),
                     GL_STREAM_COPY);

        glBindBuffer(GL_COPY_WRITE_BUFFER, positionBuffer);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 4 * sizeof(float) * positionTypes.size());

        glDeleteBuffers(1, &tmpBuffer);
    }
}
}
