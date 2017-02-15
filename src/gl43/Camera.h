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
 * @file Camera.h
 * @brief << brief description >>
 * @author clonker
 * @date 13.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#ifndef PROJECT_CAMERA_H
#define PROJECT_CAMERA_H

#include "common43.h"

namespace rv {
class Camera {
public:
    Camera();

    ~Camera();

    glm::mat4 getViewMatrix() const;

    const glm::vec3 &position() const;

    void rotate(const float xangle, const float yangle);

    void zoom(const float value);

    void movex(const float value);

    void movey(const float value);

    void setPosition(const glm::vec3 &pos);

private:
    glm::vec3 _pos;
    glm::quat _rot;
    float _xangle;
    float _yangle;
};

}


#endif //PROJECT_CAMERA_H
