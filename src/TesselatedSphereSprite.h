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
 * @file TesselatedPointSprite.h
 * @brief << brief description >>
 * @author clonker
 * @date 2/14/18
 */



#pragma once

#include "common.h"

namespace rv {

class SphereTesselation {
public:

    using vertices_type = std::vector<std::array<GLshort, 3>>;
    using indices_type = std::vector<std::array<GLushort, 3>>;
    using normals_type = std::vector<std::array<GLshort, 3>>;

    explicit SphereTesselation(std::size_t recursionDepth);

    const vertices_type &vertices() const{
        return _vertices;
    }

    const indices_type &indices() const {
        return _indices;
    }

    const normals_type &normals() const {
        return _normals;
    }

private:
    vertices_type _vertices;
    indices_type _indices;
    normals_type _normals;

    void subdivide();
};

class TesselatedSphereSprite {
public:

    TesselatedSphereSprite();

    ~TesselatedSphereSprite();

    void render(GLuint instances);

    void setPositionBuffer(GLuint buffer, GLsizei stride = 0, GLintptr offset = 0);

private:
    union {
        struct {
            GLuint vertexBuffer;
            GLuint indexBuffer;
            GLuint normalsBuffer;
        };
        GLuint buffers[3];
    };
    GLuint vertexArray;

    SphereTesselation _sphere;
};

}

