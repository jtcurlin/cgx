// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/common.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <variant>

namespace cgx::core
{

enum class ItemType
{
    Asset,
    Node
};

std::string translate_item_typename(ItemType type);

constexpr std::size_t k_invalid_id = 0;

class Item : public std::enable_shared_from_this<Item>
{
public:
    explicit Item(ItemType item_type, const std::string& tag, const std::string& path = "");
    virtual  ~Item();

    void set_tag(const std::string& tag);
    // void set_label(const std::string &label);
    void set_path(const std::string &path);

    std::size_t        get_id() const;
    const std::string& get_tag() const;
    // const std::string& get_label() const;
    const std::string& get_path() const;

    const ItemType& get_item_type() const;
    const std::string& get_item_typename() const;

    virtual std::string get_path_prefix() const;


protected:
    // UID<Item>   m_id{};
    std::size_t m_id{k_invalid_id};

    std::string m_tag{};                // name, i.e. MyNode

    std::string m_path{};               // unique path
    std::string m_source_path{};

    ItemType m_item_type;
    std::string m_item_typename{};

    static std::size_t generate_id();
};
}
