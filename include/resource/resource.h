// Copyright © 2024 Jacob Curlin

#pragma once

#include <string>
#include <mutex>

namespace cgx::resource
{
    class ResourceManager;

    using RUID = size_t;
    const RUID k_invalid_id = 0;

    enum class ResourceType
    {
        Mesh,
        Model,
        Material,
        Texture,
        Shader
    };

    class Resource
    {
    public:
        virtual ~Resource() = default;

        virtual ResourceType getType() const = 0;

        RUID getID() const                      { return m_id; }
        bool isActive() const                   { return m_id != k_invalid_id; }
        std::string getTag() const              { return m_tag; }
        void setTag(const std::string& tag)     { m_tag = tag; }
        bool hasTag() const                     {return !m_tag.empty(); }

        std::string getPath() const             { return m_path; }
        void setPath(const std::string& path)   { m_path = path; }

    protected:
        Resource(const std::string& path, const std::string& tag = "")
            : m_tag(tag), m_path(path)
        {}

    private:
        friend class ResourceManager;
        void setID(RUID id) { m_id = id; };

        RUID m_id;
        std::string m_tag; 
        std::string m_path;

        
        /*
        // derive a name for a resource from a filepath
        static std::string deriveNameFromPath(const std::string& path)
        {
            auto last_slash_pos = path.find_last_of("/\\");
            std::string file_name;
            if (last_slash_pos != std::string::npos)
            {
                file_name = path.substr(last_slash_pos + 1);
            }
            else
            {
                file_name = path;
            }

            auto last_dot_pos = file_name.find_last_of('.');
            if (last_dot_pos != std::string::npos)
            {
                return file_name.substr(0, last_dot_pos);
            }

            return file_name;
        }
        */

    }; // class Resource

} // namespace cgx::resource