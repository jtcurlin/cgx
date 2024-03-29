// Copyright © 2024 Jacob Curlin

#pragma once

#include "ecs/common.h"


namespace cgx::events::engine
{
    const cgx::ecs::EventId QUIT = "Events::Engine::QUIT"_hash;
    const cgx::ecs::EventId ENABLE_CAMERA_CONTROL = "Events::Engine::ENABLE_CAMERA_CONTROL"_hash;
    const cgx::ecs::EventId DISABLE_CAMERA_CONTROL = "Events::Engine::DISABLE_CAMERA_CONTROL"_hash;
}

namespace cgx::events::camera
{
    const cgx::ecs::EventId MOVE = "cgx::events::camera::move"_hash;

    namespace move
    {
        const cgx::ecs::ParamId DIRECTION = "cgx::events::camera::move::direction"_hash;
    }

}

namespace cgx::events::ecs
{
    const cgx::ecs::EventId ENTITY_CREATED = "cgx::events::ecs::ENTITY_CREATED"_hash;
    const cgx::ecs::EventId ENTITY_DESTROYED = "cgx::events::ecs::ENTITY_DESTROYED"_hash;
    const cgx::ecs::ParamId ENTITY_ID = "cgx::events::ecs::ENTITY_ID"_hash;
}

namespace cgx::events::resource
{
    const cgx::ecs::EventId RESOURCE_REGISTERED = "cgx::events::resource::RESOURCE_REGISTERED"_hash;
    const cgx::ecs::EventId RESOURCE_UNREGISTERED = "cgx::events::resource::RESOURCE_UNREGISTERED"_hash;

    const cgx::ecs::ParamId RESOURCE_UID = "cgx::events::resource::RESOURCE_UID"_hash;
    const cgx::ecs::ParamId RESOURCE_TYPE = "cgx::events:resource::RESOURCE_TYPE"_hash;
    const cgx::ecs::ParamId RESOURCE_PATH = "cgx::events:resource::RESOURCE_PATH"_hash;
    const cgx::ecs::ParamId RESOURCE_TAG = "cgx::events:resource::RESOURCE_TAG"_hash;
}