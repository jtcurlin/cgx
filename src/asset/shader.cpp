// Copyright © 2024 Jacob Curlin

#include "asset/shader.h"
#include "utility/error.h"

#include "glad/glad.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace cgx::asset
{
Shader::Shader(std::string tag, std::string source_path)
    : Asset(tag, get_path_prefix() + tag, source_path)
{
    const fs::path    shader_root_path{m_external_path};
    const std::string base_shader_filename = shader_root_path.filename().string();
    const fs::path    v_path               = shader_root_path / (base_shader_filename + ".vs");
    const fs::path    f_path               = shader_root_path / (base_shader_filename + ".fs");

    if (!exists(v_path)) {
        CGX_ERROR("Shader: Vertex shader does not exist at derived path [{}].", m_vert_path);
        m_initialized = false;
    }
    else if (!exists(f_path)) {
        CGX_ERROR("Shader: Fragment shader does not exist at derived path [{}].", m_frag_path);
        m_initialized = false;
    }
    else {
        std::ifstream vert_fs, frag_fs;

        vert_fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        frag_fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            std::stringstream vert_ss, frag_ss;

            vert_fs.open(v_path.string());
            frag_fs.open(f_path.string());

            vert_ss << vert_fs.rdbuf();
            frag_ss << frag_fs.rdbuf();

            vert_fs.close();
            frag_fs.close();

            m_vert_code = vert_ss.str();
            m_frag_code = frag_ss.str();
        }
        catch (const std::exception& e) {
            CGX_ERROR("Shader: File not successfully read: {}", e.what());
        }

        m_initialized = init();
    }
}

Shader::Shader(std::string tag, std::string vertex_code, std::string fragment_code)
    : Asset(tag, get_path_prefix() + tag, get_path_prefix() + tag )
    , m_vert_code{std::move(vertex_code)}
    , m_frag_code{std::move(fragment_code)}

{
    m_initialized = init();
}

Shader::~Shader()
{
    glDeleteShader(m_program_id);
    CGX_CHECK_GL_ERROR;
}

bool Shader::init()
{
    bool success = true;

    // strings -> c-strings
    const char* vert_code_cstr = m_vert_code.c_str();
    const char* frag_code_cstr = m_frag_code.c_str();

    // build / compile vertex shader
    const uint32_t vert_id = glCreateShader(GL_VERTEX_SHADER);
    CGX_CHECK_GL_ERROR;
    glShaderSource(vert_id, 1, &vert_code_cstr, nullptr);
    CGX_CHECK_GL_ERROR;
    glCompileShader(vert_id);
    CGX_CHECK_GL_ERROR;
    if (!check_compile_errors(vert_id, "VERTEX")) {
        success = false;
    }

    // build / compile fragment shader
    const uint32_t frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    CGX_CHECK_GL_ERROR;
    glShaderSource(frag_id, 1, &frag_code_cstr, nullptr);
    CGX_CHECK_GL_ERROR;
    glCompileShader(frag_id);
    CGX_CHECK_GL_ERROR;
    if (!check_compile_errors(frag_id, "FRAGMENT")) {
        success = false;
    }

    // build / compile shader program
    m_program_id = glCreateProgram();
    glAttachShader(m_program_id, vert_id);
    CGX_CHECK_GL_ERROR;
    glAttachShader(m_program_id, frag_id);
    CGX_CHECK_GL_ERROR;
    glLinkProgram(m_program_id);
    CGX_CHECK_GL_ERROR;
    if (!check_compile_errors(m_program_id, "PROGRAM")) {
        success = false;
    }

    // delete individual shaders since no longer needed with program now built
    glDeleteShader(vert_id);
    CGX_CHECK_GL_ERROR;
    glDeleteShader(frag_id);
    CGX_CHECK_GL_ERROR;

    return success;
}

void Shader::use() const
{
    glUseProgram(m_program_id);
    CGX_CHECK_GL_ERROR;
}

bool Shader::check_compile_errors(const unsigned int shader, const std::string& type)
{
    int  success;
    char info_log[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, info_log);
            CGX_ERROR("Shader: Compilation error of type {} \n {}", type, info_log);
            return false;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, info_log);
            CGX_ERROR("Shader: Linking error of type {} \n {}", type, info_log);
            return false;
        }
    }
    return true;
}

std::string Shader::get_path_prefix() const
{
    return Item::get_path_prefix() + "/" + AssetType::get_lower_typename(AssetType::Shader) + "/";
}

AssetType::Type Shader::get_asset_type() const
{
    return AssetType::Shader;
}

void Shader::set_bool(const std::string& name, const bool value) const
{
    glUniform1i(glGetUniformLocation(m_program_id, name.c_str()), static_cast<GLint>(value));
    CGX_CHECK_GL_ERROR;
}

void Shader::set_int(const std::string& name, const int value) const
{
    glUniform1i(glGetUniformLocation(m_program_id, name.c_str()), value);
    CGX_CHECK_GL_ERROR;
}

void Shader::set_float(const std::string& name, const float value) const
{
    glUniform1f(glGetUniformLocation(m_program_id, name.c_str()), value);
    CGX_CHECK_GL_ERROR;
}

void Shader::set_vec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(m_program_id, name.c_str()), 1, &value[0]);
    CGX_CHECK_GL_ERROR;
}

void Shader::set_vec2(const std::string& name, const float x, const float y) const
{
    glUniform2f(glGetUniformLocation(m_program_id, name.c_str()), x, y);
    CGX_CHECK_GL_ERROR;
}

void Shader::set_vec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_program_id, name.c_str()), 1, &value[0]);
    CGX_CHECK_GL_ERROR;
}

void Shader::set_vec3(const std::string& name, const float x, const float y, const float z) const
{
    glUniform3f(glGetUniformLocation(m_program_id, name.c_str()), x, y, z);
    CGX_CHECK_GL_ERROR;
}

void Shader::set_vec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(m_program_id, name.c_str()), 1, &value[0]);
    CGX_CHECK_GL_ERROR;
}

void Shader::set_vec4(const std::string& name, const float x, const float y, const float z, const float w) const
{
    glUniform4f(glGetUniformLocation(m_program_id, name.c_str()), x, y, z, w);
    CGX_CHECK_GL_ERROR;
}

void Shader::set_mat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    CGX_CHECK_GL_ERROR;
}

void Shader::set_mat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    CGX_CHECK_GL_ERROR;
}

void Shader::set_mat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    CGX_CHECK_GL_ERROR;
}

}
