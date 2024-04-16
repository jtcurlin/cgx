// Copyright © 2024 Jacob Curlin

#include "core/hierarchy.h"
#include "utility/logging.h"

namespace cgx::core
{
Hierarchy::Hierarchy(const ItemType item_type, const std::string& tag)
    : Item(item_type, tag)
{
    set_internal_path(Hierarchy::get_path_prefix() + tag);
}

Hierarchy::~Hierarchy() = default;

void Hierarchy::remove()
{
    std::shared_ptr<Hierarchy> parent = m_parent.lock();
    while (!m_children.empty()) {
        const auto child = m_children.back();
        child->set_parent(parent);
        child->update_path_recursive();
    }

    set_parent({});
}

void Hierarchy::recursive_remove()
{
    while (!m_children.empty()) {
        m_children.back()->recursive_remove();
    }

    set_parent({});
}

void Hierarchy::recursive_remove_children()
{
    while (!m_children.empty()) {
        m_children.back()->recursive_remove();
    }
}

void Hierarchy::on_add_child(const std::shared_ptr<Hierarchy>& child, std::size_t position)
{
    CGX_VERIFY(child.get() != this);

    const auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        CGX_ERROR("[todo: enhance log message] node already has child.");
        return;
    }

    position = std::min(m_children.size(), position);
    m_children.insert(m_children.begin() + position, child);
    update_path_recursive();
}

void Hierarchy::on_remove_child(Hierarchy* const child)
{
    CGX_VERIFY(child != nullptr);

    const auto it = std::remove_if(
        m_children.begin(),
        m_children.end(),
        [child](const std::shared_ptr<Hierarchy>& item) {
            return item.get() == child;
        });

    if (it != m_children.end()) {
        m_children.erase(it, m_children.end());
    }
    else {
        CGX_ERROR("[todo: enhance log message] couldn't remove node from parent");
    }
}

void Hierarchy::set_parent(const std::shared_ptr<Hierarchy>& parent)
{
    set_parent(parent, std::numeric_limits<std::size_t>::max());
}

void Hierarchy::set_parent(const std::shared_ptr<Hierarchy>& new_parent, const std::size_t position)
{
    CGX_VERIFY(new_parent.get() != this);

    Hierarchy* old_parent  = m_parent.lock().get();
    m_parent               = new_parent;
    Hierarchy* curr_parent = m_parent.lock().get();

    if (old_parent == curr_parent) {
        return;
    }

    // keep this entry alive
    // (1) while removing from old parents children before being added to new parent's, or
    // (2) [todo]
    auto shared_this = std::static_pointer_cast<Hierarchy>(weak_from_this().lock());

    if (old_parent) {
        old_parent->on_remove_child(this);
    }

    if (new_parent) {
        new_parent->on_add_child(shared_this, position);
    }
    else {
        CGX_TRACE("[todo: enhance log message] orphan");
        update_path_recursive();
    }

    set_depth_recursive(new_parent ? new_parent->get_depth() + 1 : 0);
}

void Hierarchy::set_parent(Hierarchy* parent)
{
    set_parent(parent, std::numeric_limits<std::size_t>::max());
}

void Hierarchy::set_parent(Hierarchy* const parent, const std::size_t position)
{
    CGX_VERIFY(parent != this);
    if (parent != nullptr) {
        set_parent(parent->get_shared(), position);
    }
    else {
        set_parent(std::shared_ptr<Hierarchy>{}, position);
    }
}

void Hierarchy::update_path_recursive()
{
    if (auto parent = m_parent.lock()) {
        m_internal_path = parent->get_internal_path() + "/" + m_tag;
    }
    else {
        m_internal_path = Hierarchy::get_path_prefix() + m_tag;
    }

    for (const auto& child : m_children) {
        child->update_path_recursive();
    }
}

void Hierarchy::set_depth_recursive(const std::size_t depth)
{
    if (m_depth == depth) {
        return;
    }
    m_depth = depth;
    for (const auto& child : m_children) {
        CGX_VERIFY(child.get() != this);
        child->set_depth_recursive(depth + 1);
    }
}

std::shared_ptr<Hierarchy> Hierarchy::get_shared()
{
    return std::static_pointer_cast<Hierarchy>(shared_from_this());
}

std::weak_ptr<Hierarchy> Hierarchy::get_parent() const
{
    return m_parent;
}

std::size_t Hierarchy::get_depth() const
{
    return m_depth;
}

const std::vector<std::shared_ptr<Hierarchy>>& Hierarchy::get_children() const
{
    return m_children;
}

std::vector<std::shared_ptr<Hierarchy>>& Hierarchy::get_mutable_children()
{
    return m_children;
}

std::weak_ptr<Hierarchy> Hierarchy::get_root()
{
    const auto& parent = get_parent().lock();
    if (!parent) {
        return std::static_pointer_cast<Hierarchy>(shared_from_this());
    }
    return parent->get_root();
}

std::size_t Hierarchy::get_child_count() const
{
    return m_children.size();
}

std::size_t Hierarchy::get_index_in_parent() const
{
    if (auto parent = get_parent().lock()) {
        const auto idx = parent->get_index_of_child(this);
        return idx.has_value() ? idx.value() : 0;
    }
    CGX_ASSERT(false, "todo");
}

std::optional<std::size_t> Hierarchy::get_index_of_child(const Hierarchy* child) const
{
    for (std::size_t i = 0, end = m_children.size() ; i < end ; ++i) {
        if (m_children[i].get() == child) {
            return i;
        }
    }
    return {};
}

bool Hierarchy::is_ancestor(const Hierarchy* candidate) const
{
    const auto& parent = get_parent().lock();

    if (!parent) {
        return false;
    }
    if (parent.get() == candidate) {
        return true;
    }

    return parent->is_ancestor(candidate);
}


void Hierarchy::for_each(const std::function<bool (Hierarchy& hierarchy)>& func)
{
    if (!func(*this)) {
        return;
    }
    for (const auto& child : m_children) {
        child->for_each(func);
    }
}
}
