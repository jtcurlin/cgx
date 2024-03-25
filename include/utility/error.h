// Copyright © 2024 Jacob Curlin

#pragma once

#include "utility/logging.h"
#include <glad/glad.h>
#include <string>

namespace cgx::render
{
    inline void CheckGLError(const char* stmt, const char* fname, int line)
    {
        GLenum err = glGetError();
        while (err != GL_NO_ERROR)
        {
            std::string error;
            switch(err)
            {
                case GL_INVALID_OPERATION:              error =  "INVALID_OPERATION"; break;
                case GL_INVALID_ENUM:                   error = "INVALID_ENUM"; break;
                case GL_INVALID_VALUE:                  error = "INVALID_VALUE"; break;
                case GL_OUT_OF_MEMORY:                  error = "OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION"; break;
                default:                                error = "UNKNOWN_ERROR";
            }

            CGX_ERROR("[OpenGL Error] ({}) in {} {} {}", error, stmt, fname, line);
            err = glGetError();
        }
    }

} // namespace cgx::render

#ifndef CGX_CHECK_GL_ERROR
#define CGX_CHECK_GL_ERROR cgx::render::CheckGLError(__FUNCTION__, __FILE__, __LINE__);
#endif
