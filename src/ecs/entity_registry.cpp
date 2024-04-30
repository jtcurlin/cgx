// Copyright © 2024 Jacob Curlin

#include "ecs/entity_registry.h"

namespace cgx::ecs
{

EntityRegistry::EntityRegistry()
    : m_active_entity_count(0) // initialize vector of MAX_ENTITIES
{
    for (Entity entity = 0 ; entity < MAX_ENTITIES ; ++entity) {
        m_available_entities.push(entity);
    }
}

EntityRegistry::~EntityRegistry() = default;

Entity EntityRegistry::acquire_entity() // fetch first unused entity from entity vector
{
    CGX_ASSERT(m_active_entity_count < MAX_ENTITIES, "Too many active entities.");

    const Entity id = m_available_entities.front(); // fetch
    m_available_entities.pop();
    ++m_active_entity_count;

    return id;
}

void EntityRegistry::release_entity(Entity entity)
{
    CGX_ASSERT(entity < MAX_ENTITIES, "Entity out of range.");

    m_signatures[entity].reset(); // reset entity's signature (bitset)

    m_available_entities.push(entity);

    --m_active_entity_count;
}

void EntityRegistry::set_signature(const Entity entity, const Signature signature)
{
    CGX_ASSERT(entity < MAX_ENTITIES, "Entity out of range.");

    m_signatures[entity] = signature;
}

Signature EntityRegistry::get_signature(const Entity entity)
{
    CGX_ASSERT(entity < MAX_ENTITIES, "Entity out of range.");

    return m_signatures[entity];
}

std::vector<Entity> EntityRegistry::get_active_entities() const
{
    std::vector<Entity> active_entities;
    for (Entity entity = 0 ; entity < MAX_ENTITIES ; ++entity) {
        if (m_signatures[entity].any()) {
            active_entities.push_back(entity);
        }
    }
    return active_entities;
}
}
