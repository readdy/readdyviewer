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
 * @file Texture.cpp
 * @brief << brief description >>
 * @author clonker
 * @date 14.02.17
 * @copyright GNU Lesser General Public License v3.0
 */

#include "Texture.h"
#include <png.h>
#include <fstream>

namespace rv {
Texture::Texture() {
    glGenTextures(1, &texture);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);
}

void Texture::bind(GLenum target) const {
    glBindTexture(target, texture);
}

/** libpng I/O helper callback.
 * File I/O callback helper for using libpng with STL file streams.
 * \param png_ptr libpng handle
 * \param data memory to which to put the data
 * \param length number of bytes to read
 *
 */
void _PngReadFn(png_structp png_ptr, png_bytep data, png_size_t length) {
    std::ifstream *file = reinterpret_cast<std::ifstream *> (png_get_io_ptr(png_ptr));
    file->read(reinterpret_cast<char *> (data), length);
    if (file->fail())
        png_error(png_ptr, "I/O error.");
}


void Texture::load(const GLenum &target, const std::string &fname, GLenum internalFormat) {
    // open the file
    std::ifstream file(fname.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!file.is_open())
        throw std::runtime_error(std::string("Cannot open texture: ") + fname);

    png_structp png_ptr;
    png_infop info_ptr;

    // initialize libpng
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                     NULL, NULL, NULL);
    if (!png_ptr)
        throw std::runtime_error("Cannot create png read struct");
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        throw std::runtime_error("Cannot create png info struct.");
    }

    // long jump error handling
    if (setjmp (png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        throw std::runtime_error("libpng error.");
    }

    // set I/O callback
    png_set_read_fn(png_ptr, &file, _PngReadFn);

    // read header information and request a usable format
    png_read_info(png_ptr, info_ptr);
    png_set_packing(png_ptr);
    png_set_expand(png_ptr);
    if (png_get_bit_depth(png_ptr, info_ptr) == 16)
        png_set_swap(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    // obtain information about the image
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    int channels = png_get_channels(png_ptr, info_ptr);
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    int depth = png_get_bit_depth(png_ptr, info_ptr);

    // assure a valid pixel depth
    if (depth != 8 && depth != 16 && depth != 32) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        throw std::runtime_error(std::string("Invalid bit depth in ") + fname);
    }

    // convert depth to OpenGL data type
    const GLenum types[] = {
            GL_UNSIGNED_BYTE,
            GL_UNSIGNED_SHORT,
            GL_ZERO,
            GL_UNSIGNED_INT
    };
    GLenum type = types[(depth / 8) - 1];

    // assure a valid number of channels
    if (channels < 1 || channels > 4) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        throw std::runtime_error(std::string("Invalid number of channels.") + fname);
    }

    // convert the number of channels to an OpenGL format
    const GLenum formats[] = {
            GL_RED, GL_RG, GL_RGB, GL_RGBA
    };
    GLenum format = formats[channels - 1];

    // read the image data
    std::vector<GLubyte> data;
    data.resize(rowbytes * height);
    int i;
    for (i = 0; i < height; i++) {
        png_read_row(png_ptr, &data[i * rowbytes], NULL);
    }

    // cleanup libpng
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    // pass the image data to OpenGL
    glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data.data());
}


}
