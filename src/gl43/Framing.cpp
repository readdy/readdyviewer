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
 * @file Framing.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 14.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include "Framing.h"

namespace rv {

Framing::Framing() {
    program.compileShader(GL_VERTEX_SHADER, "shaders/gl43/framing/vertex.glsl");
    program.compileShader(GL_FRAGMENT_SHADER, "shaders/gl43/framing/fragment.glsl");
    program.compileShader(GL_FRAGMENT_SHADER, "shaders/gl43/light/light.glsl");
    program.link();
    GL_CHECK_ERROR()
    program.use();
    GL_CHECK_ERROR()
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(4, buffers);

    // store vertices to a buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    const GLfloat vertices[] = {
            -100, 0, 100,
            100, 0, 100,
            100, 0, -100,
            -100, 0, -100
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // define the vertices as vertex attribute 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // store texture coordinates to a buffer object
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    const GLushort texcoords[] = {
            0, 10,
            10, 10,
            10, 0,
            0, 0
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
    // define texture coordinates as vertex attribute 1
    glVertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // store normals to a buffer object
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    const GLbyte normals[] = {
            0, 127, 0, 0,
            0, 127, 0, 0,
            0, 127, 0, 0,
            0, 127, 0, 0
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    // define normals as vertex attribute 2
    glVertexAttribPointer(2, 3, GL_BYTE, GL_TRUE, 4, 0);
    glEnableVertexAttribArray(2);

    // store indices to a buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    const GLushort indices[] = {
            0, 1, 2,
            0, 2, 3
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // load the texture
    texture.bind(GL_TEXTURE_2D);
    if(HIDDEN_FRANK_MODE == 1) {
        Texture::load(GL_TEXTURE_2D, "textures/Frank_Noe.png", GL_COMPRESSED_RGB);
    } else {
        Texture::load(GL_TEXTURE_2D, "textures/framing.png", GL_COMPRESSED_RGB);
    }
    // generate a mipmap and enable trilinear filtering
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Framing::~Framing() {
    glDeleteBuffers(4, buffers);
    glDeleteVertexArrays(1, &vao);
}

void Framing::render() {
    // activate shader program
    GL_CHECK_ERROR()
    program.use();
    GL_CHECK_ERROR()
    // bind texture, vertex array and index buffer
    texture.bind(GL_TEXTURE_2D);
    GL_CHECK_ERROR()
    glBindVertexArray(vao);
    GL_CHECK_ERROR()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    GL_CHECK_ERROR()
    // render the framing
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    GL_CHECK_ERROR()
}
}