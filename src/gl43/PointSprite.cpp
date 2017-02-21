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
 * @file PointSprites.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 13.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include "PointSprite.h"

namespace rv {

PointSprite::PointSprite() {
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
    glVertexAttribPointer(0, 2, GL_SHORT, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(0);

    const GLushort indices[] = {
            0, 2, 1,
            2, 0, 3
    };

    // store indices to a buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

PointSprite::~PointSprite() {
    glDeleteBuffers (sizeof(buffers) / sizeof(buffers[0]), buffers);
    glDeleteVertexArrays (1, &vertexArray);
}

void PointSprite::setPositionBuffer(GLuint buffer, GLsizei stride, GLintptr offset) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offset);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1,1);
}

void PointSprite::setHighlightBuffer(GLuint buffer, GLsizei stride, GLintptr offset) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, stride, (const void *) offset);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
}

void PointSprite::render(GLuint instances) const {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, instances);
}

}