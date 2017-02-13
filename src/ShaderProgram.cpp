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

const GLuint &ShaderProgram::get() const {
    return program;
}

void ShaderProgram::compileShader(GLenum type, const std::string &fname, const std::string &header) {
    GLuint shader;

    std::vector<char> data;
    GLint length = 0;

    // prepend header
    data.assign(header.begin(), header.end());

    // fix line count
    const std::string linedef = "\n#line 1\n";
    data.insert(data.end(), linedef.begin(), linedef.end());

    // load shader source
    length = static_cast<GLint>(data.size());
    {
        std::ifstream f(fname.c_str(), std::ios_base::in);
        if (!f.is_open())
            throw std::runtime_error(std::string("Cannot load shader: ") + fname);

        f.seekg(0, std::ios_base::end);
        auto len = f.tellg();
        f.seekg(0, std::ios_base::beg);

        data.resize(static_cast<std::size_t>(length + len));
        f.read(&data[length], len);
        len = f.gcount();

        if (f.bad())
            throw std::runtime_error(std::string("Cannot load shader: ") + fname);
        length += len;
    }

    // create a shader, specify the source and attempt to compile it
    shader = glCreateShader(type);
    const GLchar *src = (const GLchar *) &data[0];
    glShaderSource(shader, 1, &src, &length);
    glCompileShader(shader);

    // check the compilation status and throw the error log as exception on failure
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE.m_value) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        data.resize(static_cast<std::size_t>(length));
        glGetShaderInfoLog(shader, length, NULL, &data[0]);
        glDeleteShader(shader);
        throw std::runtime_error(std::string("Cannot compile shader \"") + fname + "\":"
                                 + std::string(&data[0], static_cast<std::size_t>(length)));
    }

    // attach the shader object to the program
    glAttachShader(program, shader);
    // delete the shader object (it is internally stored as long as the program is not deleted)
    glDeleteShader(shader);

}
}

