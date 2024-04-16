// Copyright © 2024 Jacob Curlin

#pragma once

#include <filesystem>
#include <string>

namespace cgx::core
{
enum class ItemType
{
    Asset,
    Node
};

std::string translate_item_typename(ItemType item_type);

constexpr std::size_t k_invalid_id = 0;

class Item : public std::enable_shared_from_this<Item>
{
public:
    explicit Item(ItemType item_type, std::string tag);
    virtual  ~Item();

    void set_tag(const std::string& tag);
    void set_internal_path(const std::string &internal_path);
    void set_external_path(const std::string &external_path);

    std::size_t        get_id() const;
    const std::string& get_tag() const;
    const std::string& get_internal_path() const;
    const std::string& get_external_path() const;

    const ItemType& get_item_type() const;
    const std::string& get_item_typename() const;

    virtual std::string get_path_prefix() const;

protected:
    std::size_t m_id{k_invalid_id};

    std::string m_tag{};

    std::string m_internal_path{};
    std::string m_external_path{};

    ItemType m_item_type;
    std::string m_item_typename{};

    static std::size_t generate_id();
};
}
