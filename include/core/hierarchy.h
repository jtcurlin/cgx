// Copyright © 2024 Jacob Curlin

#pragma once

#include "core/item.h"

#include <functional>
#include <optional>
#include <vector>

namespace cgx::core
{
class Hierarchy : public Item
{
public:
    explicit Hierarchy(ItemType item_type, const std::string& tag);
    ~Hierarchy() override;

    /*
    explicit Hierarchy(const Hierarchy& src);
    Hierarchy& operator=(const Hierarchy& src);
    Hierarchy(const std::string_view name);
    */

    virtual void set_parent(const std::shared_ptr<Hierarchy>& parent);
    virtual void set_parent(const std::shared_ptr<Hierarchy>& new_parent, std::size_t position);
    virtual void on_add_child(const std::shared_ptr<Hierarchy>& child, std::size_t position);
    virtual void on_remove_child(Hierarchy* child);
    // virtual void handle_parent_update(Hierarchy* old_parent, Hierarchy new_parent);

    std::shared_ptr<Hierarchy> get_shared();

    std::weak_ptr<Hierarchy>                       get_parent() const;
    std::size_t                                    get_depth() const;
    const std::vector<std::shared_ptr<Hierarchy>>& get_children() const;
    std::vector<std::shared_ptr<Hierarchy>>&       get_mutable_children();
    std::weak_ptr<Hierarchy>                       get_root();

    std::size_t                get_child_count() const;
    std::size_t                get_index_in_parent() const;
    std::optional<std::size_t> get_index_of_child(const Hierarchy* child) const;
    bool                       is_ancestor(const Hierarchy* candidate) const;

    void set_parent(Hierarchy* parent);
    void set_parent(Hierarchy* parent, std::size_t position);
    void set_depth_recursive(std::size_t depth);

    void update_path_recursive();

    void remove();
    void recursive_remove();
    void recursive_remove_children();

    void for_each(const std::function<bool(Hierarchy& hierarchy)>& func);

    template<typename T>
    void for_each(const std::function<bool(const T& node)>& func) const
    {
        const T* item = dynamic_cast<const T*>(this);
        if (item != nullptr) {
            if (!func(*item)) {
                return;
            }
        }

        for (const auto& child : m_children) {
            child->for_each(func);
        }
    }

protected:
    std::weak_ptr<Hierarchy>                m_parent;
    std::vector<std::shared_ptr<Hierarchy>> m_children;
    std::size_t                             m_depth{0};

};
}
