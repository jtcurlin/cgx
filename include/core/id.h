// Copyright © 2024 Jacob Curlin

// a thread safe, type-safe unique identifier (UID) system 
// with optional string <-> id associations & bi-directional lookup.

#pragma once

#include <atomic>
#include <cstddef>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace cgx::core
{
    // registry class for managing UID <-> string associations
    class UIDRegistry
    {
    public:

        // associates a UID with a string
        template<typename T>
        static void associate(size_t uid, const std::string& str)
        {
            std::lock_guard<std::mutex> lock(s_registry_mutex);
            getMap<T>()[uid] = str;
            getReverseMap<T>()[str] = uid;
        }

        // lookup string associated with UID, return nullopt if no association exists
        template<typename T>
        static std::optional<std::string> lookup(size_t uid)
        {
            std::lock_guard<std::mutex> lock(s_registry_mutex);
            auto& map = getMap<T>;
            auto it = map.find(uid);
            if (it != map.end())
            {
                return it->second;
            }
            else
            {
                return std::nullopt;
            }
        }

        // lookup UID associated with a string, return nullopt if no association exists
        template<typename T>
        static std::optional<size_t> lookup(const std::string& str)
        {
            std::lock_guard<std::mutex> lock(s_registry_mutex);
            auto& reverse_map = getReverseMap<T>();
            auto it = reverse_map.find(str);
            if (it != reverse_map.end())
            {
                return it->second;
            }
            else 
            {
                return std::nullopt;
            }
        }

    private:
        // retreive map for UID->string associations
        template<typename T>
        static std::unordered_map<size_t, std::string>& getMap()
        {
            static std::unordered_map<size_t, std::string> s_map;
            return s_map;
        }

        // retreive map for string->UID associations
        template<typename T>
        static std::unordered_map<std::string, size_t>& getReverseMap()
        {
            static std::unordered_map<std::string, size_t> s_reverse_map;
            return s_reverse_map;
        }

        static std::mutex s_registry_mutex;

    }; // class UIDRegistry

    std::mutex UIDRegistry::s_registry_mutex;

    // templated UID class for generating unique identifiers w/ optional string associations
    template<typename T>
    class UID
    {
    public:
        // construct a UID with or without an associated string
        UID () : m_id(generate()) {}
        UID(const std::string& str) : m_id(generate(str)) {}

        // implicit UID->size_t conversion 
        operator size_t() const { return m_id; }

    private:
        size_t m_id; // the unique identifier
        static std::atomic<size_t> s_next_id; // atomic counter for next available id

        // thread-safe generation of a new UID w/o string association
        static size_t generate() 
        { 
            return s_next_id.fetch_add(1, std::memory_order_relaxed);
        }

        // thread-safe generation of a new UID w/ string association
        static size_t generate(const std::string& str)
        {
            size_t id = generate();
            UIDRegistry::associate<T>(id, str);
            return id;
        }

    }; // class UID

    template<typename T>
    std::atomic<size_t> UID<T>::s_next_id(0);


} // namespace cgx::core