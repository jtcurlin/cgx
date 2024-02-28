// Copyright © 2024 Jacob Curlin

#ifndef SANDBOX_H
#define SANDBOX_H

#include "cpgx.h"
#include "cubemap.h"
#include <string>


#define AMBIENT_MAP_BIT 1
#define DIFFUSE_MAP_BIT 2
#define SPECULAR_MAP_BIT 4
#define NORMAL_MAP_BIT 8


class Sandbox : public Engine
{

public:
    Sandbox();              
    ~Sandbox();     

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Shutdown() override;

    void ImguiRender();

    std::string message;
private:

    std::vector<std::string> model_filenames;
    std::unordered_map<std::string, std::shared_ptr<cgx::graphics::Model>> loaded_models;

    std::vector<std::string> shader_names;
    std::unordered_map<std::string, std::shared_ptr<cgx::graphics::Shader>> loaded_shaders;

    std::unique_ptr<cgx::graphics::Cubemap> m_skybox;

};

#endif // SANDBOX_H