// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/system.h"

namespace cgx::component
{
struct Transform;
struct Collider;
}

namespace cgx::physics
{

class CollisionSystem final : public ecs::System
{
public:
    explicit CollisionSystem(ecs::ECSManager* ecs_manager);
    ~CollisionSystem() override;

    void frame_update(float dt) override;
    void fixed_update(float dt) override;

    void on_entity_added(ecs::Entity entity) override;
    void on_entity_removed(ecs::Entity entity) override;

private:
    bool check_collision(const component::Transform& t1, const component::Collider& c1,
        const component::Transform& t2, const component::Collider& c2);

    void resolve_collision(ecs::Entity e1, ecs::Entity e2);

};

}
