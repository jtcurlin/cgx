// Copyright © 2024 Jacob Curlin

#pragma once

#include "cgx.h"

class Sandbox final : public cgx::core::Engine
{
public:
    Sandbox();
    ~Sandbox() override;

protected:
    void initialize() override;
    void update() override;
    void render() override;

    void geometry_test() const;

    void load_assets() const;
};
