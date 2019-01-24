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
 * @date 2/15/17
 */
#ifndef PROJECT_COMMON_H
#define PROJECT_COMMON_H

#include <sstream>
#include <random>

#include <glbinding/gl43/gl.h>
#include <glbinding/Binding.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <fmt/format.h>

#include <spdlog/fmt/ostr.h>

using namespace gl;

namespace rv {

using edges_type = std::vector<std::vector<std::tuple<std::size_t, std::size_t>>>;

extern GLFWwindow *window;

inline std::ostream& operator<<(std::ostream& os, gl::GLenum value) {
    os << static_cast<unsigned int>(value);
    return os;
}

#define GL_CHECK_ERROR() { \
GLenum err = glGetError(); \
if (err != GL_NO_ERROR) { \
std::stringstream ss; \
ss << "OpenGL error detected at " << __FILE__ << " (line: " << __LINE__ << ") 0x" << std::hex << err << std::endl; \
throw std::runtime_error(ss.str()); \
} \
} \

namespace log {

template<typename... Args>
void trace(Args &&... args) {
    auto consoleLog = spdlog::get("console");
    if (!consoleLog) {
        consoleLog = spdlog::stdout_color_mt("console");
        consoleLog->set_level(spdlog::level::debug);
    }
    consoleLog->trace(std::forward<Args>(args)...);
}

template<typename... Args>
void debug(Args &&... args) {
    auto consoleLog = spdlog::get("console");
    if (!consoleLog) {
        consoleLog = spdlog::stdout_color_mt("console");
        consoleLog->set_level(spdlog::level::debug);
    }
    consoleLog->debug(std::forward<Args>(args)...);
}

template<typename... Args>
void error(Args &&... args) {
    auto consoleLog = spdlog::get("console");
    if (!consoleLog) {
        consoleLog = spdlog::stdout_color_mt("console");
        consoleLog->set_level(spdlog::level::debug);
    }
    consoleLog->error(std::forward<Args>(args)...);
}
}

namespace rnd {
template<typename RealType=double, typename Generator = std::default_random_engine>
RealType normal(const RealType mean = 0.0, const RealType variance = 1.0) {
    static thread_local Generator generator(clock() + std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::normal_distribution<RealType> distribution(mean, variance);
    return distribution(generator);
}

template<typename RealType=double, typename Generator = std::default_random_engine>
RealType uniform_real(const RealType a = 0.0, const RealType b = 1.0) {
    static thread_local Generator generator(clock() + std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_real_distribution<RealType> distribution(a, b);
    return distribution(generator);
}

template<typename IntType=int, typename Generator = std::default_random_engine>
IntType uniform_int(const IntType a, const IntType b) {
    static thread_local Generator generator(clock() + std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_int_distribution<IntType> distribution(a, b);
    return distribution(generator);
}

template<typename RealType=double, typename Generator = std::default_random_engine>
RealType exponential(RealType lambda = 1.0) {
    static thread_local Generator generator(clock() + std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::exponential_distribution<RealType> distribution(lambda);
    return distribution(generator);
}

template<typename Generator = std::default_random_engine>
glm::vec3 normal3(const double mean = 0.0, const double variance = 1.0) {
    return {normal<double, Generator>(mean, variance),
            normal<double, Generator>(mean, variance),
            normal<double, Generator>(mean, variance)};
}

template<typename Iter, typename Gen = std::default_random_engine>
Iter random_element(Iter start, const Iter end) {
    using IntType = typename std::iterator_traits<Iter>::difference_type;
    std::advance(start, uniform_int<IntType, Gen>(0, std::distance(start, end)));
    return start;
}
}
}
#endif //PROJECT_COMMON_H
