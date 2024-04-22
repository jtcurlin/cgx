// Copyright © 2024 Jacob Curlin

#include "../../include/ecs/event.h"

namespace cgx::ecs
{
EventId Event::get_type() const
{
    return m_type;
}
}
