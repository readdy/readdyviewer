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
#include "ShaderProgram.h"
#include "Trajectory.h"
#include "PointSprite.h"
#include "Framing.h"
#include "LightArrangement.h"
#include "Edges.h"
#include "preprocessing.h"

namespace rv {
class Viewer {
public:
    Viewer(rv::TrajectoryEntries entries, const std::vector<Light> &lights, const TrajectoryConfiguration& config);

    ~Viewer();

    void onMouseMove(double x, double y);
    void onMouseDown(int button);
    void onMouseUp(int button);
    void onKeyUp(int key);
    void onKeyDown(int key);
    void resize(unsigned int width, unsigned int height);

    bool frame();

private:
    using transformation_buffer_t = struct transformation_buffer {
        glm::mat4 viewmat {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        glm::mat4 projmat {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        glm::mat4 invviewmat {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    };

    void updateViewMatrix();

    union {
        struct {
            GLuint transformationBuffer;
        };
        GLuint buffers[1];
    };

    Framing framing;
    Trajectory trajectory;
    PointSprite pointSprite;
    Edges edgeSprite;
    ShaderProgram particleProgram;
    ShaderProgram edgeProgram;
    Camera camera;
    LightArrangement lightArrangement;

    bool running, interrupt;
    glm::mat4 projmat;
    unsigned int width;
    unsigned int height;
    unsigned int wait;
    unsigned int currentWait;
    double last_time;
    double last_fps_time;
    float guitimer;
    unsigned int framecount;
    unsigned int fps;
    bool drawPeriodic;


};
}


#endif //PROJECT_VIEWER_H
