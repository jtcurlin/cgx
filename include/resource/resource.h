// Copyright © 2024 Jacob Curlin

#pragma once

#include <string>
#include <mutex>

namespace cgx::resource
{
    using RUID = size_t;  // resource unique-identifiers are size_t's

    struct ResourceMetadata
    {
        RUID ruid = 0;
        std::string type = "";
        std::string name = "";
        std::string source_path = "";
        std::string derived_path = "";
    };

    class Resource
    {
    public:
        virtual ~Resource() = default;

        virtual std::string getTypeName() const = 0;

        RUID getRUID() const                   { return m_ruid; }
        ResourceMetadata getMetadata() const    { return {m_ruid, getTypeName(), m_name, m_source_path, m_derived_path}; }

        std::string getName() const             { return m_name; }
        void setName(const std::string& name)   { m_name = name; }

        std::string getSourcePath() const             { return m_source_path; }
        void setSourcePath(const std::string& path)   { m_source_path = path; }

        std::string getDerivedPath() const             { return m_derived_path; }
        void setDerivedPath(const std::string& path)   { m_derived_path = path; }

    protected:
        Resource(const std::string& source_path, const std::string& derived_path, const std::string& name = "")
            : m_source_path(source_path), m_derived_path(derived_path), m_name(name)
        { 
            m_ruid = GenerateRUID();
            if (m_name.empty())
            {
                m_name = deriveNameFromPath(derived_path);
            }
        }

        Resource(const std::string& name)
            : m_name(name)
        {
            m_ruid = GenerateRUID();
        }

    private:
        size_t m_ruid;
        
        std::string m_name;
        std::string m_source_path = "";
        std::string m_derived_path = "";

        // generate a unique resource ID
        static RUID GenerateRUID() { 
            static RUID s_next_ruid = 0;
            static std::mutex s_ruid_mutex;
            std::lock_guard<std::mutex> lock(s_ruid_mutex);
            return s_next_ruid++;
        }

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

    }; // class Resource

} // namespace cgx::resource