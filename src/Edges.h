/********************************************************************
 * Copyright © 2018 Computational Molecular Biology Group,          *
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
 * @file Edges.h
 * @brief << brief description >>
 * @author clonker
 * @date 2/2/18
 */



#pragma once

#include "common.h"
#include "ShaderProgram.h"
#include "Trajectory.h"

namespace rv {
class Edges {
public:
    using edge_size_type = GLfloat;

    Edges();
    ~Edges();
    void render(GLuint instances);

    void setEdgesBuffer(GLuint buffer, GLsizei stride = 0, GLintptr offset = 0);

    void setEdgeColorBuffer(GLuint buffer, GLsizei stride = 0, GLintptr offset = 0);

    edge_size_type &edgeSize() {
        return _edgeSize;
    }

    const edge_size_type &edgeSize() const {
        return _edgeSize;
    }
private:

    union {
        struct {
            GLuint edgeBuffer;
            GLuint edgeColorBuffer;
        };
        GLuint buffers[2];
    };

    ShaderProgram program;
    GLuint vao;
    edge_size_type _edgeSize;
};
}

