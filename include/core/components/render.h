// Copyright © 2024 Jacob Curlin

#pragma once

#include "asset/model.h"
#include "asset/shader.h"

namespace cgx::component
{
struct Render
{
    std::shared_ptr<asset::Model>  model;
    std::shared_ptr<asset::Shader> shader;
    bool visible{true};
};
}
