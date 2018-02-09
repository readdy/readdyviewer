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
 * @file Edges.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 2/2/18
 */


#include "Edges.h"

namespace rv {

template<unsigned int N>
auto cylinderVertices() {
    const GLdouble step = 2.0 * 3.14159265358979323846 / static_cast<double>(N);
    std::array<GLshort, 2*3*N> vertices {};
    std::array<GLushort, 2*3*N+6> indices {};
    std::array<GLshort, 2*3*N> normals {};

    auto x = [&](std::uint32_t n) { return 32767 * std::cos(n * step); };
    auto y = [&](std::uint32_t n) { return 32767 * std::sin(n * step); };

    for(auto i = 0U; i < N; ++i) {
        vertices[3*i + 0] = x(i);
        vertices[3*i + 1] = y(i);
        vertices[3*i + 2] = 32767;
        normals[3*i + 0] = x(i);
        normals[3*i + 1] = y(i);
        normals[3*i + 2] = 0;
    }
    for(auto i = 0U; i < N; ++i) {
        vertices[3*N + 3*i + 0] = x(i);
        vertices[3*N + 3*i + 1] = y(i);
        vertices[3*N + 3*i + 2] = -32767;
        normals[3*N + 3*i + 0] = x(i);
        normals[3*N + 3*i + 1] = y(i);
        normals[3*N + 3*i + 2] = 0;
    }

    for(auto i = 0U; i < N+1; ++i) {
        indices[6*i + 0] = i % N;
        indices[6*i + 1] = (i % N) + N;
        indices[6*i + 2] = ((i+1) % N)+N;
        indices[6*i + 3] = i % N;
        indices[6*i + 5] = (i+1) % N;
        indices[6*i + 4] = ((i+1)%N)+N;
    }

    return std::make_tuple(vertices, indices, normals);
};

struct Edges::Impl {
    decltype(cylinderVertices<20>()) cylinder = cylinderVertices<20U>();
};

Edges::Edges() : _edgeSize(1.f), pimpl(std::make_unique<Impl>()) {

    const auto &cylinderVertices = std::get<0>(pimpl->cylinder);
    const auto &cylinderIndices = std::get<1>(pimpl->cylinder);
    const auto &normals = std::get<2>(pimpl->cylinder);

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glGenBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
    // store vertices to a buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(GLshort), cylinderVertices.data(), GL_STATIC_DRAW);
    // define the vertices as vertex attribute 0
    glVertexAttribPointer(0, 3, GL_SHORT, GL_TRUE, 0, nullptr);
    glEnableVertexAttribArray(0);

    {
        glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLshort), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_SHORT, GL_TRUE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinderIndices.size() * sizeof(GLushort), cylinderIndices.data(),
                     GL_STATIC_DRAW);
    }
}

void Edges::render(GLuint instances) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glDrawElementsInstanced(GL_TRIANGLES, std::get<1>(pimpl->cylinder).size(), GL_UNSIGNED_SHORT, nullptr, instances);
}

Edges::~Edges() {
    glDeleteBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
    glDeleteVertexArrays(1, &vertexArray);
}

void Edges::setEdgesFromBuffer(GLuint buffer, GLsizei stride, GLintptr offset) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offset);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
}

void Edges::setEdgesToBuffer(GLuint buffer, GLsizei stride, GLintptr offset) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offset);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);
}

void Edges::setEdgeColorBuffer(GLuint buffer, GLsizei stride, GLintptr offset) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_TRUE, stride, (const void *) offset);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);
}

}
