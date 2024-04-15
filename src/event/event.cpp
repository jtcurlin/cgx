// Copyright © 2024 Jacob Curlin

#include "event/event.h"

namespace cgx::event
{
EventId Event::get_type() const
{
    return m_type;
}
}
