// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "asset/model.h"
#include "asset/shader.h"

namespace cgx::component
{
struct Render
{
    std::shared_ptr<asset::Model>  model;
    std::shared_ptr<asset::Shader> shader;
};
}
