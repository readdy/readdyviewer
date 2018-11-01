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

namespace fmt {
template <>
struct formatter<glm::vec3> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const glm::vec3 &p, FormatContext &ctx) {
        return format_to(ctx.begin(), "({:.1f}, {:.1f}, {:.1f})", p.x, p.y, p.z);
    }
};

template <>
struct formatter<glm::quat> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const glm::quat &p, FormatContext &ctx) {
        return format_to(ctx.begin(), "({:.1f}, {:.1f}, {:.1f}, {:.1f})", p.x, p.y, p.z, p.w);
    }
};
}

namespace rv {
Camera::Camera() : _xangle(0), _yangle(0), _rot {0, 0, 0, 0}, _pos {0, 0, 0} {
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
    _rot = glm::rotate (glm::quat (0, 0, 0, 1), _yangle * float (M_PI / 180.0f), glm::vec3 (0.0f, 1.0f, 0.0f));
    _rot = glm::rotate (_rot, _xangle * float (M_PI / 180.0f), glm::vec3 (1.0f, 0.0f, 0.0f));
    log::trace("rot {}, pos {}", _rot, _pos);
}

void Camera::zoom(const float value) {
    _pos += 0.1f * value * (_rot * glm::vec3 (0, 0, 1));
    log::trace("zoom {}, pos {}", value, _pos);
}

void Camera::movex(const float value) {
    _pos += 0.05f * value * (_rot * glm::vec3 (1, 0, 0));
    log::trace("movex {}, pos {}", value, _pos);
}

void Camera::movey(const float value) {
    _pos += 0.05f * value * (_rot * glm::vec3 (0, 1, 0));
    log::trace("movey {}, pos {}", value, _pos);
}

void Camera::setPosition(const glm::vec3 &pos) {
    _pos = pos;
    log::trace("setpos {}", _pos);
}

Camera::~Camera() = default;

}
