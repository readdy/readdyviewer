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
 * @file Framing.h
 * @brief << brief description >>
 * @author clonker
 * @date 14.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#ifndef PROJECT_FRAMING_H
#define PROJECT_FRAMING_H

#include "common.h"
#include "Texture.h"
#include "ShaderProgram.h"

namespace rv {
class Framing {
public:
    Framing(const std::string& resourceDir);
    ~Framing();
    void render();
private:

    ShaderProgram program;

    union {
        struct {
            GLuint vertexBuffer;
            GLuint normalBuffer;
            GLuint texCoordBuffer;
            GLuint indexBuffer;
        };
        GLuint buffers[4];
    };
    GLuint vao;
    Texture texture;

};

}


#endif //PROJECT_FRAMING_H
