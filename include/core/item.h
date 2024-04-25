// Copyright © 2024 Jacob Curlin

#pragma once

#include <string>
#include <memory>

namespace cgx::core
{
struct ItemType
{
    enum Type
    {
        Asset,
        Node,
        Hierarchy
    };

    static std::string get_typename(Type type);
    static std::string get_lower_typename(Type type);
};

constexpr std::size_t k_invalid_id = 0;

class Item : public std::enable_shared_from_this<Item>
{
public:
    explicit Item(std::string tag, std::string external_path = "", std::string internal_path = "");
    virtual ~Item();

    void set_tag(const std::string& tag);
    void set_internal_path(const std::string& internal_path);
    void set_external_path(const std::string& external_path);

    std::size_t         get_id() const;
    const std::string&  get_tag() const;
    const std::string&  get_internal_path() const;
    const std::string&  get_external_path() const;

    virtual ItemType::Type get_item_type() const = 0;
    virtual std::string    get_item_typename() const;

    virtual std::string get_path_prefix() const;

protected:
    std::size_t m_id{k_invalid_id};

    std::string m_tag{};

    std::string m_internal_path{};
    std::string m_external_path{};

private:
    static std::size_t generate_id();
};
}
