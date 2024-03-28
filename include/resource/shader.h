// Copyright © 2024 Jacob Curlin

#pragma once

#include "resource/resource.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "utility/error.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace cgx::resource
{
    class Shader : public Resource
    {
    public:

        // Shader(const std::string& shader_dir_path, const std::string& vert_filename, const std::string& frag_filename);       // constructor
        Shader(const std::string& shader_root_dir, const std::string& shader_name);
        ~Shader();                                                      

        virtual std::string getTypeName() const override { return "Shader"; }

        bool isInitialized() const { return m_initialized; }

        bool Initialize();
        void Use() const;
        void Log() const;

    private:
        bool m_initialized = false;

        unsigned int m_program_id;        // shader program reference m_program_id
        std::string m_vert_path;
        std::string m_frag_path;

        bool checkCompileErrors(unsigned int shader, std::string type);

    public:
        // uniform setter utility functions
        void setBool(const std::string &name, bool value) const
        {
            glUniform1i(glGetUniformLocation(m_program_id, name.c_str()), (int)value); CGX_CHECK_GL_ERROR;
        }
        void setInt(const std::string &name, int value) const
        {
            glUniform1i(glGetUniformLocation(m_program_id, name.c_str()), value); CGX_CHECK_GL_ERROR;
        }
        void setFloat(const std::string &name, float value) const
        {
            glUniform1f(glGetUniformLocation(m_program_id, name.c_str()), value); CGX_CHECK_GL_ERROR;
        }
        void setVec2(const std::string &name, const glm::vec2 &value) const
        { 
            glUniform2fv(glGetUniformLocation(m_program_id, name.c_str()), 1, &value[0]); CGX_CHECK_GL_ERROR;
        }
        void setVec2(const std::string &name, float x, float y) const
        { 
            glUniform2f(glGetUniformLocation(m_program_id, name.c_str()), x, y); CGX_CHECK_GL_ERROR;
        }
        // ------------------------------------------------------------------------
        void setVec3(const std::string &name, const glm::vec3 &value) const
        { 
            glUniform3fv(glGetUniformLocation(m_program_id, name.c_str()), 1, &value[0]); CGX_CHECK_GL_ERROR;
        }
        void setVec3(const std::string &name, float x, float y, float z) const
        { 
            glUniform3f(glGetUniformLocation(m_program_id, name.c_str()), x, y, z); CGX_CHECK_GL_ERROR;
        }
        // ------------------------------------------------------------------------
        void setVec4(const std::string &name, const glm::vec4 &value) const
        { 
            glUniform4fv(glGetUniformLocation(m_program_id, name.c_str()), 1, &value[0]); CGX_CHECK_GL_ERROR;
        }
        void setVec4(const std::string &name, float x, float y, float z, float w) const
        { 
            glUniform4f(glGetUniformLocation(m_program_id, name.c_str()), x, y, z, w); CGX_CHECK_GL_ERROR;
        }
        // ------------------------------------------------------------------------
        void setMat2(const std::string &name, const glm::mat2 &mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]); CGX_CHECK_GL_ERROR;
        }
        // ------------------------------------------------------------------------
        void setMat3(const std::string &name, const glm::mat3 &mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]); CGX_CHECK_GL_ERROR;
        }
        // ------------------------------------------------------------------------
        void setMat4(const std::string &name, const glm::mat4 &mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]); CGX_CHECK_GL_ERROR;
        }

    }; // class Shader

} // namespace cgx::resource