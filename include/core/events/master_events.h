// Copyright © 2024 Jacob Curlin

#pragma once
#include "core/event.h"

namespace cgx::core::event::master
{
constexpr EventId TOGGLE_INTERFACE_MODE = "event::master::TOGGLE_INTERFACE_MODE"_hash;
constexpr EventId TOGGLE_CONTROL_MODE   = "event::master::TOGGLE_CONTROL_MODE"_hash;

constexpr EventId ACTIVATE_GUI_CONTROL_MODE  = "event::master::ACTIVATE_GUI_CONTROL_MODE"_hash;
constexpr EventId ACTIVATE_GAME_CONTROL_MODE = "event::master::ACTIVATE_GAME_CONTROL_MODE"_hash;

constexpr EventId ACTIVATE_GUI_INTERFACE  = "event::master::ACTIVATE_GUI_INTERFACE"_hash;
constexpr EventId ACTIVATE_GAME_INTERFACE = "event::master::ACTIVATE_GAME_INTERFACE"_hash;

constexpr EventId QUIT = "event::master::QUIT"_hash;
}
