// Copyright © 2024 Jacob Curlin

#include "core/item.h"

#include <unordered_map>

namespace cgx::core
{

std::string ItemType::get_typename(const Type type)
{
    static const std::unordered_map<Type, std::string> type_names = {
        {Asset, "Asset"}, {Node, "Node"}, {Hierarchy, "Hierarchy"}
    };
    const auto it = type_names.find(type);
    return it != type_names.end() ? it->second : "Unknown";
}

std::string ItemType::get_lower_typename(const Type type)
{
    static const std::unordered_map<Type, std::string> type_names = {
        {Asset, "asset"}, {Node, "node"}, {Hierarchy, "hierarchy"}
    };
    const auto it = type_names.find(type);
    return it != type_names.end() ? it->second : "unknown";
}

Item::Item(std::string tag, std::string internal_path, std::string external_path)
    : m_tag(std::move(tag))
    , m_external_path(std::move(external_path))
    , m_internal_path(std::move(internal_path))
    , m_id(generate_id()) {}

Item::~Item() = default;

void Item::set_tag(const std::string& tag)
{
    m_tag = tag;
}

void Item::set_internal_path(const std::string& internal_path)
{
    m_internal_path = internal_path;
}

void Item::set_external_path(const std::string& external_path)
{
    m_external_path = external_path;
}

std::size_t Item::get_id() const
{
    return m_id;
}

const std::string& Item::get_tag() const
{
    return m_tag;
}

const std::string& Item::get_internal_path() const
{
    return m_internal_path;
}

const std::string& Item::get_external_path() const
{
    return m_external_path;
}

std::string Item::get_item_typename() const
{
    return ItemType::get_typename(get_item_type());
}


std::string Item::get_path_prefix() const
{
    return "cgx://item";
}

std::size_t Item::generate_id()
{
    static std::size_t last_id = 1;
    return ++last_id;
}
}
