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
 * @file PointSprites.h
 * @brief << brief description >>
 * @author clonker
 * @date 13.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#ifndef PROJECT_POINTSPRITES_H
#define PROJECT_POINTSPRITES_H

#include "common.h"

namespace rv {
class PointSprite {
public:
    PointSprite();
    ~PointSprite();

    void setPositionBuffer(GLuint buffer, GLsizei stride = 0, GLintptr offset = 0);
    void setHighlightBuffer(GLuint buffer, GLsizei stride = 0, GLintptr offset = 0);
    void render(GLuint instances) const;

private:
    union {
        struct {
            GLuint vertexBuffer;
            GLuint indexBuffer;
        };
        GLuint buffers[2];
    };
    GLuint vertexArray;
};

}


#endif //PROJECT_POINTSPRITES_H
