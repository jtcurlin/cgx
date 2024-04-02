// Copyright © 2024 Jacob Curlin

#include "resource/shader.h"
#include "utility/error.h"

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

namespace cgx::resource
{
    Shader::Shader(const std::string& path, const std::string& tag)
        : Resource(path, tag)
    {
        fs::path shader_root_path {path};

        std::string base_shader_filename = shader_root_path.filename().string();

        fs::path temp_vpath = shader_root_path / (base_shader_filename + ".vs"); // generate vertex shader's full path
        fs::path temp_fpath = shader_root_path / (base_shader_filename + ".fs"); // generate fragment shader's full path

        m_vert_path = temp_vpath.string();  // explicitly convert path to std::string
        m_frag_path = temp_fpath.string();  // explicitly convert path to std::string

        if (!fs::exists(m_vert_path))
        {
            CGX_ERROR("Shader: Vertex shader does not exist at derived path [{}].", m_vert_path);
            m_initialized = false;
        }
        else if (!fs::exists(m_frag_path))
        {
            CGX_ERROR("Shader: Fragment shader does not exist at derived path [{}].", m_frag_path);
            m_initialized = false;
        }
        else
        {
            m_initialized = Initialize();
        }
    }

    Shader::~Shader() 
    {
        glDeleteShader(m_program_id); CGX_CHECK_GL_ERROR;
    }

    bool Shader::Initialize()
    {
        std::string vertCode, fragCode;       // strings for holding shader code

        std::ifstream vShaderFile, fShaderFile;     // filestreams for reading in shader code

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            // open files w/ shader code
            vShaderFile.open(m_vert_path);
            fShaderFile.open(m_frag_path);

            // read file contents into stringstreams
            std::stringstream vShaderStream;
            std::stringstream fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            // close files
            vShaderFile.close();
            fShaderFile.close();

            // convert & store code within string variables
            vertCode = vShaderStream.str();
            fragCode = fShaderStream.str();

        }
        catch(const std::exception& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
            return false;
        }

        bool success = true;

        // strings -> c-strings
        const char* vShaderCode = vertCode.c_str();
        const char* fShaderCode = fragCode.c_str();

        // shader reference ID's
        unsigned int vert_id;
        unsigned int frag_id;
        
        // build / compile vertex shader
        vert_id = glCreateShader(GL_VERTEX_SHADER); CGX_CHECK_GL_ERROR;
        glShaderSource(vert_id, 1, &vShaderCode, nullptr); CGX_CHECK_GL_ERROR;
        glCompileShader(vert_id); CGX_CHECK_GL_ERROR;
        if (!checkCompileErrors(vert_id, "VERTEX")) { success = false; }

        // build / compile fragment shader
        frag_id = glCreateShader(GL_FRAGMENT_SHADER); CGX_CHECK_GL_ERROR;
        glShaderSource(frag_id, 1, &fShaderCode, nullptr); CGX_CHECK_GL_ERROR;
        glCompileShader(frag_id); CGX_CHECK_GL_ERROR;
        if (!checkCompileErrors(frag_id, "FRAGMENT")) { success = false; }

        // build / compile shader program
        m_program_id = glCreateProgram();
        glAttachShader(m_program_id, vert_id); CGX_CHECK_GL_ERROR;
        glAttachShader(m_program_id, frag_id); CGX_CHECK_GL_ERROR;
        glLinkProgram(m_program_id); CGX_CHECK_GL_ERROR;
        if (!checkCompileErrors(m_program_id, "PROGRAM")) { success = false; }

        // delete individual shaders since no longer needed with program now built
        glDeleteShader(vert_id); CGX_CHECK_GL_ERROR;
        glDeleteShader(frag_id); CGX_CHECK_GL_ERROR;

        return success;
    }

    void Shader::Use() const
    {
        glUseProgram(m_program_id); CGX_CHECK_GL_ERROR;
    }

    bool Shader::checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR:SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                    << "\n -- ---------------------------- -- " << std::endl;
                return false;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
                    << "\n -- ---------------------------- -- " << std::endl;
                return false;
            }
        }
        return true;
    }

    void Shader::Log() const
    {
        CGX_DEBUG("Shader {}: m_vert_path = {}", getTag(), m_vert_path);
        CGX_DEBUG("Shader {}: m_frag_path = {}", getTag(), m_frag_path);
    }

} // namespace cgx::resource