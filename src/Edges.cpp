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

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);

    program.compileShader(GL_VERTEX_SHADER, "shaders/edge/vertex.glsl");
    program.compileShader(GL_FRAGMENT_SHADER, "shaders/edge/fragment.glsl");
    program.link();
    GL_CHECK_ERROR()
    program.use();
    GL_CHECK_ERROR()

}

void Edges::render(GLuint instances) {
    glBindVertexArray(vao);
    GL_CHECK_ERROR()
    glDepthMask(false);
    GL_CHECK_ERROR()
    glEnableVertexAttribArray(0);
    GL_CHECK_ERROR()
    program.use();
    GL_CHECK_ERROR()
    auto edgeSize = _edgeSize;
    GL_CHECK_ERROR()
    if (_edgeSize > 1) edgeSize = edgeSize * 2 - 1;
    glLineWidth(1.0);
    GL_CHECK_ERROR()
    glDepthRange(0.01f, 1.0f);
    GL_CHECK_ERROR()
    glDrawArrays(GL_LINES, 0, instances);
    GL_CHECK_ERROR()
    glDepthRange(0, 1);
    GL_CHECK_ERROR()
    glDepthMask(true);
    GL_CHECK_ERROR()
    glDisableVertexAttribArray(0);
    GL_CHECK_ERROR()
}

Edges::~Edges() {
    glDeleteBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
    glDeleteVertexArrays(1, &vao);
}

void Edges::setEdgesBuffer(GLuint buffer, GLsizei stride, GLintptr offset) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offset);
    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 1);
}

void Edges::setEdgeColorBuffer(GLuint buffer, GLsizei stride, GLintptr offset) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offset);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
}

}
