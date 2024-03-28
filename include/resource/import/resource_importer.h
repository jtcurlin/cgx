// Copyright © 2024 Jacob Curlin

#pragma once

#include "resource/resource.h"

#include <string>
#include <vector>

namespace cgx::resource
{

    class ResourceImporter
    {
    public:
        virtual ~ResourceImporter() = default;

        virtual void Initialize(const std::string& path) = 0;

        virtual RUID Import() = 0;

        virtual std::string getTypeName() const = 0;

    protected:
        // errors? store last error? etc. 

    }; // class ResourceImporter

} // namespace cgx::resource