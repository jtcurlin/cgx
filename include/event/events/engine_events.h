// Copyright © 2024 Jacob Curlin

#pragma once

#include "event/event.h"

namespace cgx::events::engine
{
    const cgx::event::EventId QUIT = "Events::Engine::QUIT"_hash;
    const cgx::event::EventId ENABLE_CAMERA_CONTROL = "Events::Engine::ENABLE_CAMERA_CONTROL"_hash;
    const cgx::event::EventId DISABLE_CAMERA_CONTROL = "Events::Engine::DISABLE_CAMERA_CONTROL"_hash;
}

namespace cgx::events::camera
{
    const cgx::event::EventId MOVE = "cgx::events::camera::move"_hash;

    namespace move
    {
        const cgx::event::ParamId DIRECTION = "cgx::events::camera::move::direction"_hash;
    }

}

namespace cgx::events::ecs
{
    const cgx::event::EventId ENTITY_CREATED = "cgx::events::ecs::ENTITY_CREATED"_hash;
    const cgx::event::EventId ENTITY_DESTROYED = "cgx::events::ecs::ENTITY_DESTROYED"_hash;
    const cgx::event::ParamId ENTITY_ID = "cgx::events::ecs::ENTITY_ID"_hash;
}

namespace cgx::events::resource
{
    const cgx::event::EventId RESOURCE_REGISTERED = "cgx::events::resource::RESOURCE_REGISTERED"_hash;
    const cgx::event::EventId RESOURCE_UNREGISTERED = "cgx::events::resource::RESOURCE_UNREGISTERED"_hash;

    const cgx::event::ParamId RESOURCE_UID = "cgx::events::resource::RESOURCE_UID"_hash;
    const cgx::event::ParamId RESOURCE_TYPE = "cgx::events:resource::RESOURCE_TYPE"_hash;
    const cgx::event::ParamId RESOURCE_PATH = "cgx::events:resource::RESOURCE_PATH"_hash;
    const cgx::event::ParamId RESOURCE_TAG = "cgx::events:resource::RESOURCE_TAG"_hash;
}