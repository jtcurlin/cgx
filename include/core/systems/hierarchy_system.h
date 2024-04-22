// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/system.h"
#include "core/components/hierarchy.h"
#include <unordered_set>

namespace cgx::core
{
class HierarchySystem final : public ecs::System
{
public:
    explicit HierarchySystem(ecs::ECSManager* ecs_manager);
    ~HierarchySystem() override;

    void update(float dt) override;

    void on_entity_added(ecs::Entity entity) override;
    void on_entity_removed(ecs::Entity entity) override;

    void on_parent_update(ecs::Entity child, ecs::Entity old_parent, ecs::Entity new_parent);

    void sort_order_by_depth();
    void dfs_children(ecs::Entity entity, std::unordered_set<ecs::Entity>& visited);

    [[nodiscard]] const std::vector<ecs::Entity>& get_order() const;

private:
    std::vector<ecs::Entity> m_order{};
};
}
