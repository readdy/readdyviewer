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

Edges::Edges() : _edgeSize(1.f) {

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glGenBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
    GLshort vertices[] = {
            -32767, 32767,
            32767, 32767,
            32767, -32767,
            -32767, -32767
    };
    // store vertices to a buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // define the vertices as vertex attribute 0
    glVertexAttribPointer(0, 2, GL_SHORT, GL_TRUE, 0, nullptr);
    glEnableVertexAttribArray(0);

    const GLushort indices[] = {
            0, 2, 1,
            2, 0, 3
    };

    // store indices to a buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}

void Edges::render(GLuint instances) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr, instances);
}

Edges::~Edges() {
    glDeleteBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
    glDeleteVertexArrays(1, &vertexArray);
}

void Edges::setEdgesFromBuffer(GLuint buffer, GLsizei stride, GLintptr offset) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offset);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
}

void Edges::setEdgesToBuffer(GLuint buffer, GLsizei stride, GLintptr offset) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offset);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
}

void Edges::setEdgeColorBuffer(GLuint buffer, GLsizei stride, GLintptr offset) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, stride, (const void *) offset);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);
}

}
