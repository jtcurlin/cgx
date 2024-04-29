// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/asset.h"

#include "glm/glm.hpp"
#include <string>

namespace cgx::gui
{
class PropertiesPanel;
}

namespace cgx::asset
{

struct ShaderType
{
    enum Type
    {
        PBR,
        Phong,
        Cubemap,
        Unknown
    };
};

class Shader final : public Asset
{
    friend class gui::PropertiesPanel;

public:
    Shader(std::string tag, std::string source_path, ShaderType::Type type);
    Shader(std::string tag, std::string vertex_code, std::string fragment_code, ShaderType::Type type);
    ~Shader() override;

    bool is_initialized() const
    {
        return m_initialized;
    }

    bool init();
    void use() const;

    ShaderType::Type get_type() const;
    void set_type(ShaderType::Type type);

private:
    ShaderType::Type m_type;
    bool             m_initialized = false;

    unsigned int m_program_id{0};
    std::string  m_vert_path{};
    std::string  m_frag_path{};
    std::string  m_vert_code{};
    std::string  m_frag_code{};

    static bool check_compile_errors(unsigned int shader, const std::string& type);

    std::string     get_path_prefix() const override;
    AssetType::Type get_asset_type() const override;

public:
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;

    void set_vec2(const std::string& name, const glm::vec2& value) const;
    void set_vec3(const std::string& name, const glm::vec3& value) const;
    void set_vec4(const std::string& name, const glm::vec4& value) const;

    void set_vec2(const std::string& name, float x, float y) const;
    void set_vec3(const std::string& name, float x, float y, float z) const;
    void set_vec4(const std::string& name, float x, float y, float z, float w) const;

    void set_mat2(const std::string& name, const glm::mat2& mat) const;
    void set_mat3(const std::string& name, const glm::mat3& mat) const;
    void set_mat4(const std::string& name, const glm::mat4& mat) const;
};
}
