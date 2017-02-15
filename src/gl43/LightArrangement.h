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
 * @file LightArrangement.h
 * @brief << brief description >>
 * @author clonker
 * @date 15.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#ifndef PROJECT_LIGHTARRANGEMENT_H
#define PROJECT_LIGHTARRANGEMENT_H

#include "common43.h"

namespace rv {
enum class LightType {
    AMBIENT, DIRECTION, POINT, HEAD
};

class Light {
public:
    Light(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float intensity, LightType type);
    ~Light();

    static std::size_t stride();
    void bindParams(GLuint buffer, GLuint offset) const;
private:
    using light_params_t = struct light_params {
        glm::vec4 position;
        glm::vec4 direction;
        glm::vec4 color;
        float intensity;
        GLuint type;
        float padding[2];
    };
    light_params_t params;
    GLuint buffer;
    GLuint offset;
};


class LightArrangement {
public:
    explicit LightArrangement(const std::vector<Light> &lights);
    ~LightArrangement();

    GLuint getLightsBuffer() const;

    const std::vector<Light> &getLights() const;

private:

    std::vector<Light> lights;

    union {
        struct {
            GLuint lightsBuffer;
        };
        GLuint buffers[1];
    };
};

}


#endif //PROJECT_LIGHTARRANGEMENT_H
