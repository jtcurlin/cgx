// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/event.h"

namespace cgx::core::event::asset
{
constexpr EventId ADDED    = "event::asset::ADDED"_hash;
constexpr EventId REMOVED  = "event::asset::REMOVED"_hash;
constexpr EventId MODIFIED = "event::asset::MODIFIED"_hash;

constexpr ParamId ID            = "event::asset::ID"_hash;
constexpr ParamId TYPE          = "event:asset::TYPE"_hash;
constexpr ParamId TAG           = "event:asset::TAG"_hash;
constexpr ParamId INTERNAL_PATH = "event:asset::INTERNAL_PATH"_hash;
constexpr ParamId EXTERNAL_PATH = "event:asset::EXTERNAL_PATH"_hash;
}

namespace cgx::core::event::importer
{
constexpr EventId REGISTERED   = "event::importer::REGISTERED"_hash;
constexpr EventId DEREGISTERED = "event::importer::DEREGISTERED"_hash;
constexpr EventId MODIFIED     = "event::importer::MODIFIED"_hash;

constexpr ParamId LABEL                = "event::importer::LABEL"_hash;
constexpr ParamId SUPPORTED_EXTENSIONS = "event::importer::SUPPORTED_EXTENSIONS"_hash;
}
