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
 * @file ShaderProgram.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 13.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include "ShaderProgram.h"
#include <fstream>

namespace rv {
ShaderProgram::ShaderProgram() : program(glCreateProgram()) {
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(program);
};

GLuint ShaderProgram::get() const {
    return program;
}

void ShaderProgram::compileShader(GLenum type, std::initializer_list<std::string> filenames, const std::string &header) {
    GLuint shader;

    std::vector<char> data;
    GLint length = 0;

    // prepend #version statement
    const char version[] = "#version 430 core\n";
    data.assign(version, version + (sizeof(version) / sizeof(version[0]) - 1));

    // prepend header
    data.insert(data.end(), header.begin(), header.end());

    // fix line count
    const std::string linedef = "\n#line 1\n";
    data.insert(data.end(), linedef.begin(), linedef.end());

    // load shader source
    length = static_cast<GLint>(data.size());
    for (const auto &filename : filenames) {
        size_t len;
        std::ifstream f(filename.c_str(), std::ios_base::in);
        if (!f.is_open())
            throw std::runtime_error(std::string("Cannot load shader: ") + filename);

        f.seekg(0, std::ios_base::end);
        len = static_cast<std::size_t>(f.tellg());
        f.seekg(0, std::ios_base::beg);

        data.resize(length + len);
        f.read(&data[length], len);
        len = static_cast<std::size_t>(f.gcount());

        if (f.bad())
            throw std::runtime_error(std::string("Cannot load shader: ") + filename);
        length += len;
    }

    // create a shader, specify the source and attempt to compile it
    shader = glCreateShader(type);
    const auto *src = (const GLchar *) &data[0];
    glShaderSource(shader, 1, &src, &length);
    glCompileShader(shader);

    // check the compilation status and throw the error log as exception on failure
    GLboolean status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        data.resize(static_cast<unsigned long>(length));
        glGetShaderInfoLog(shader, length, nullptr, &data[0]);
        glDeleteShader(shader);
        std::string names;
        for (const auto &fname : filenames) {
            if (!names.empty()) {
                names += ", ";
            }
            names += "\"" + fname + "\"";
        }
        throw std::runtime_error(fmt::format("Cannot compile shader {}: {}", names,
                                             std::string(&data[0], static_cast<unsigned long>(length))));
    }

    // attach the shader object to the program
    glAttachShader(program, shader);
    // delete the shader object (it is internally stored as long as the program is not deleted)
    glDeleteShader(shader);
}

void ShaderProgram::link() {
    // attempt to link the program
    glLinkProgram(program);

    // check for errors and throw the error log as exception on failure
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE.m_value) {
        GLint length;
        std::vector<char> log;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        log.resize(static_cast<std::size_t>(length));
        glGetProgramInfoLog(program, length, NULL, &log[0]);
        throw std::runtime_error(std::string("Failed to link shader program: ") +
                                 std::string(&log[0], static_cast<std::size_t>(length)));
    }
    GL_CHECK_ERROR()
}

void ShaderProgram::use() {
    glUseProgram(program);
}

GLint ShaderProgram::getUniformLocation(const char *name) const {
    return glGetUniformLocation(program, name);
}
}

