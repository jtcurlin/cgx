// Copyright © 2024 Jacob Curlin

#include "core/item.h"

namespace cgx::core
{
std::string translate_item_typename(const ItemType item_type) {
    switch(item_type) {
        case ItemType::Asset: {
            return "asset";
        }
        case ItemType::Node: {
            return "node";
        }
        default: {
            return "unknown_item_type";
        }
    }
}

Item::Item(const ItemType item_type, const std::string& tag, const std::string& path)
    : m_id(generate_id())
    , m_item_type{item_type}
    , m_item_typename{translate_item_typename(item_type)}
    , m_tag(tag)
    , m_path(path)
{}

Item::~Item() = default;

void Item::set_tag(const std::string &tag)
{
    m_tag = tag;
}

/*
void Item::set_label(const std::string &label)
{
    m_label = label;
}
*/

void Item::set_path(const std::string &path)
{
    m_path = path;
}

std::size_t Item::get_id() const
{
    return m_id;
}

const std::string& Item::get_tag() const
{
    return m_tag;
}

/*
const std::string & Item::get_label() const {
    return m_label;
}
*/

const std::string& Item::get_path() const
{
    return m_path;
}

const ItemType & Item::get_item_type() const {
    return m_item_type;
}

const std::string& Item::get_item_typename() const {
    return m_item_typename;
}

std::string Item::get_path_prefix() const
{
    return "cgx://" + get_item_typename() + "/";
}

std::size_t Item::generate_id() {
    static std::size_t last_id = 1;
    return ++last_id;
}
}
