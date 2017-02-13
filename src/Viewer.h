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
 *
 *
 * @file Viewer.h
 * @brief 
 * @author clonker
 * @date 2/12/17
 */
#ifndef PROJECT_VIEWER_H
#define PROJECT_VIEWER_H

#include "common.h"
#include "Camera.h"

namespace rv {
class Viewer {
public:
    Viewer();

    ~Viewer();

private:
    using transformation_buffer_t = struct transformation_buffer {
        glm::mat4 viewmat;
        glm::mat4 projmat;
        glm::mat4 invviewmat;
    };
    using lightparams_t = struct lightparams {
        glm::vec3 lightpos;
        float padding;
        glm::vec3 spotdir;
        float padding2;
        glm::vec3 eyepos;
        float spotexponent;
        float lightintensity;
    };

    void updateViewMatrix();

    transformation_buffer_t transformationBuffer;
    Camera camera;
    bool running;
    GLuint renderingquery;
    glm::mat4 projmat;
    unsigned int width;
    unsigned int height;
    float last_time;
    float last_fps_time;
    float guitimer;
    unsigned int framecount;
    unsigned int fps;

};
}


#endif //PROJECT_VIEWER_H
