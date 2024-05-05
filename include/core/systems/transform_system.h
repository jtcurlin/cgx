// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/system.h"
#include "core/components/transform.h"

namespace cgx::core
{
class HierarchySystem;

class TransformSystem final : public ecs::System
{
public:
    explicit TransformSystem(ecs::ECSManager* ecs_manager);
    ~TransformSystem() override;

    void initialize(HierarchySystem* hierarchy_system);

    void frame_update(float dt) override;
    void fixed_update(float dt) override;

    void on_entity_added(ecs::Entity entity) override;
    void on_entity_removed(ecs::Entity entity) override;

    void mark_children_dirty(ecs::Entity parent);

    static void update_world_matrix(component::Transform& transform, const glm::mat4& parent_matrix);

private:
    HierarchySystem* m_hierarchy_system{nullptr};
};
}
