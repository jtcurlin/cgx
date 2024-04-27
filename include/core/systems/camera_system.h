// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/system.h"

namespace cgx::core
{
class CameraSystem final : public ecs::System
{
public:
    explicit CameraSystem(ecs::ECSManager* ecs_manager);
    ~CameraSystem() override;

    void update(float dt) override;

    void on_entity_added(ecs::Entity entity) override;
    void on_entity_removed(ecs::Entity entity) override;
};
}
