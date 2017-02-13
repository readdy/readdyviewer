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
 * @file common.h
 * @brief 
 * @author clonker
 * @date 2/12/17
 */
#ifndef PROJECT_COMMON_H
#define PROJECT_COMMON_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <spdlog/spdlog.h>

using namespace gl;

namespace rv {

extern GLFWwindow *window;

namespace log {

template<typename... Args>
void trace(Args &&... args) {
    auto consoleLog = spdlog::get("console");
    if (!consoleLog) {
        spdlog::set_sync_mode();
        consoleLog = spdlog::stdout_color_mt("console");
        consoleLog->set_level(spdlog::level::debug);
    }
    consoleLog->trace(std::forward<Args>(args)...);
}

template<typename... Args>
void debug(Args &&... args) {
    auto consoleLog = spdlog::get("console");
    if (!consoleLog) {
        spdlog::set_sync_mode();
        consoleLog = spdlog::stdout_color_mt("console");
        consoleLog->set_level(spdlog::level::debug);
    }
    consoleLog->debug(std::forward<Args>(args)...);
}

template<typename... Args>
void error(Args &&... args) {
    auto consoleLog = spdlog::get("console");
    if (!consoleLog) {
        spdlog::set_sync_mode();
        consoleLog = spdlog::stdout_color_mt("console");
        consoleLog->set_level(spdlog::level::debug);
    }
    consoleLog->error(std::forward<Args>(args)...);
}
}
}


#endif //PROJECT_COMMON_H
