// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/system.h"
#include "ecs/components/transform.h"

namespace cgx::core
{
class TransformSystem final : public ecs::System
{
public:
    explicit TransformSystem(const std::shared_ptr<ecs::ComponentRegistry>& component_registry);
    ~TransformSystem() override;

    void update(float dt) override;
    void on_entity_added(ecs::Entity entity) override;
    void on_entity_removed(ecs::Entity entity) override;

    void on_component_updated(ecs::Entity entity) override;

    void   update_world_matrix(component::Transform& transform);
    void   sort_by_depth();
    size_t get_depth(ecs::Entity entity);

private:
    std::vector<ecs::Entity> m_update_order{};
};
}
