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
 * @file Camera.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 13.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include "Camera.h"
#include <cmath>

namespace rv {
Camera::Camera() : _xangle(0), _yangle(0){
}

glm::mat4 Camera::getViewMatrix() const {
    const auto direction = _rot * glm::vec3(0, 0, 1);
    const auto up = _rot * glm::vec3(0, 1, 0);
    return glm::lookAt(_pos, _pos + direction, up);
}

const glm::vec3 &Camera::position() const {
    return _pos;
}

void Camera::rotate(const float xangle, const float yangle) {
    _xangle += xangle;
    _yangle += yangle;
    _rot = glm::rotate (glm::quat (), _yangle * float (M_PI / 180.0f), glm::vec3 (0.0f, 1.0f, 0.0f));
    _rot = glm::rotate (_rot, _xangle * float (M_PI / 180.0f), glm::vec3 (1.0f, 0.0f, 0.0f));
}

void Camera::zoom(const float value) {
    _pos += 0.1f * value * (_rot * glm::vec3 (0, 0, 1));
}

void Camera::movex(const float value) {
    _pos += 0.05f * value * (_rot * glm::vec3 (1, 0, 0));
}

void Camera::movey(const float value) {
    _pos += 0.05f * value * (_rot * glm::vec3 (0, 1, 0));
}

void Camera::setPosition(const glm::vec3 &pos) {
    _pos = pos;
}

Camera::~Camera() = default;




}

