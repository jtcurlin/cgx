// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"
#include "resource/model.h"
#include "resource/shader.h"

struct RenderComponent
{
    std::shared_ptr<cgx::resource::Model> model;
    std::shared_ptr<cgx::resource::Shader> shader;
};