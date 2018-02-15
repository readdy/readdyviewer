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
 * @file TesselatedPointSprite.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 2/14/18
 */


#include "TesselatedSphereSprite.h"

namespace rv {


TesselatedSphereSprite::TesselatedSphereSprite() : _sphere(6) {
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glGenBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);

    // store vertices to a buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * _sphere.vertices().size() * sizeof(GLshort), &_sphere.vertices()[0][0],
                 GL_STATIC_DRAW);

    // define the vertices as vertex attribute 0
    glVertexAttribPointer(0, 3, GL_SHORT, GL_TRUE, 0, nullptr);
    glEnableVertexAttribArray(0);

    {
        glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
        glBufferData(GL_ARRAY_BUFFER, 3 * _sphere.normals().size() * sizeof(GLshort), &_sphere.normals()[0][0],
                     GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_SHORT, GL_TRUE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * _sphere.indices().size() * sizeof(GLushort), &_sphere.indices()[0][0],
                     GL_STATIC_DRAW);
    }
}

TesselatedSphereSprite::~TesselatedSphereSprite() {
    glDeleteBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
    glDeleteVertexArrays(1, &vertexArray);
}

template<typename T>
std::array<T, 3> crossProduct(const std::array<T, 3> &u, const std::array<T, 3> &v) {
    return {{
                    static_cast<T>(u[1] * v[2] - u[2] * v[1]),
                    static_cast<T>(u[2] * v[0] - u[0] * v[2]),
                    static_cast<T>(u[0] * v[1] - u[1] * v[0])
            }};
};

template<typename T>
T norm(const std::array<T, 3> &v) {
    return static_cast<T>(std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
}

SphereTesselation::SphereTesselation(std::size_t recursionDepth) {
    auto max = std::numeric_limits<GLshort>::max();
    auto min = std::numeric_limits<GLshort>::min();

    _vertices = {
            {{max, 0,   0}},
            {{min, 0,   0}},
            {{0,   max, 0}},
            {{0,   min, 0}},
            {{0,   0,   max}},
            {{0,   0,   min}}
    };
    _indices = {
            {{0, 4, 2}},
            {{2, 4, 1}},
            {{1, 4, 3}},
            {{3, 4, 0}},
            {{0, 2, 5}},
            {{2, 1, 5}},
            {{1, 3, 5}},
            {{3, 0, 5}}
    };

    for (std::size_t i = 1; i < recursionDepth; ++i) {
        subdivide();
    }

    _normals.resize(_vertices.size());
    for (std::size_t i = 0; i < _vertices.size(); ++i) {
        const auto &ix = _indices.at(i);
        const auto &v0 = _vertices.at(ix[0]);
        const auto &v1 = _vertices.at(ix[1]);
        const auto &v2 = _vertices.at(ix[2]);
        const std::array<GLshort, 3> l0{{static_cast<GLshort>(v1[0] - v0[0]), static_cast<GLshort>(v1[1] - v0[1]),
                                                static_cast<GLshort>(v1[2] - v0[2])}};
        const std::array<GLshort, 3> l2{{static_cast<GLshort>(v0[0] - v2[0]), static_cast<GLshort>(v0[1] - v2[1]),
                                                static_cast<GLshort>(v0[2] - v2[2])}};
        auto cross = crossProduct(l0, l2);
        auto l = norm(cross);
        cross[0] /= l;
        cross[1] /= l;
        cross[2] /= l;
        _normals.at(i) = cross;
    }
}

template<typename T>
std::vector<std::array<GLshort, 3>> midpointVertex(const T &v0, const T &v1) {
    std::vector<std::array<GLshort, 3>> midpoint;
    std::transform(v0.begin(), v0.end(), v1.begin(), std::back_inserter(midpoint), [](const auto &v1, const auto &v2) {
        return std::array<GLshort, 3>{static_cast<GLshort>((v1[0] + v2[0]) / 2),
                                      static_cast<GLshort>((v1[1] + v2[1]) / 2),
                                      static_cast<GLshort>((v1[2] + v2[2]) / 2)};
    });
    return midpoint;
}

template<typename T>
void normalize(std::vector<std::array<T, 3>> &arr) {
    for (auto &elem : arr) {
        auto len = std::sqrt(std::accumulate(elem.begin(), elem.end(), 0, [](auto current, auto x) {
            return current + x * x;
        }));
        std::transform(elem.begin(), elem.end(), elem.begin(), [len](const auto x) {
            return x / len;
        });
    }
}

void SphereTesselation::subdivide() {
    std::vector<GLushort> t0;
    std::transform(_indices.begin(), _indices.end(), std::back_inserter(t0), [](const auto &x) { return x[0]; });
    std::vector<GLushort> t1;
    std::transform(_indices.begin(), _indices.end(), std::back_inserter(t1), [](const auto &x) { return x[1]; });
    std::vector<GLushort> t2;
    std::transform(_indices.begin(), _indices.end(), std::back_inserter(t2), [](const auto &x) { return x[2]; });

    std::vector<std::array<GLshort, 3>> v0;
    std::transform(t0.begin(), t0.end(), std::back_inserter(v0), [&](const auto &x) { return _vertices[x]; });
    std::vector<std::array<GLshort, 3>> v1;
    std::transform(t1.begin(), t1.end(), std::back_inserter(v1), [&](const auto &x) { return _vertices[x]; });
    std::vector<std::array<GLshort, 3>> v2;
    std::transform(t2.begin(), t2.end(), std::back_inserter(v2), [&](const auto &x) { return _vertices[x]; });

    auto a = midpointVertex(v0, v2);
    auto b = midpointVertex(v0, v1);
    auto c = midpointVertex(v1, v2);

    normalize(a);
    normalize(b);
    normalize(c);

    _vertices.clear();

    _vertices.insert(_vertices.end(), v0.begin(), v0.end());
    _vertices.insert(_vertices.end(), b.begin(), b.end());
    _vertices.insert(_vertices.end(), a.begin(), a.end());

    _vertices.insert(_vertices.end(), b.begin(), b.end());
    _vertices.insert(_vertices.end(), v1.begin(), v1.end());
    _vertices.insert(_vertices.end(), c.begin(), c.end());

    _vertices.insert(_vertices.end(), a.begin(), a.end());
    _vertices.insert(_vertices.end(), b.begin(), b.end());
    _vertices.insert(_vertices.end(), c.begin(), c.end());

    _vertices.insert(_vertices.end(), a.begin(), a.end());
    _vertices.insert(_vertices.end(), c.begin(), c.end());
    _vertices.insert(_vertices.end(), v2.begin(), v2.end());

    _indices.resize(_vertices.size());
    std::iota(&_indices[0][0], &_indices[_indices.size() - 1][2], 0);
}

}
