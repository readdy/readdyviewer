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


#include "TesselatedPointSprite.h"

namespace rv {


TesselatedPointSprite::TesselatedPointSprite() {
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glGenBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);
}

TesselatedPointSprite::~TesselatedPointSprite() {
    glDeleteBuffers (sizeof(buffers) / sizeof(buffers[0]), buffers);
    glDeleteVertexArrays (1, &vertexArray);
}


}
