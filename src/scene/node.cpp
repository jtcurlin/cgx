// Copyright © 2024 Jacob Curlin

#include "scene/node.h"

#include <algorithm>

namespace cgx::scene
{
    Node::Node(std::string label)
        : m_label(label)
    { 
        UpdatePath();
    }

    std::string Node::getLabel() const
    {
        return m_label;
    }

    void Node::setLabel(const std::string& label)
    {
        m_label = label;
        UpdatePath();
    }

    const std::string& Node::getPath() const 
    {
        return m_path;
    }

    std::shared_ptr<Node> Node::getParent() const
    {
        return m_parent.lock();
    }
    
    void Node::setParent(const std::shared_ptr<Node>& parent)
    {
        m_parent = parent;
        UpdatePath();
    }

    void Node::addChild(std::unique_ptr<Node> child)
    {
        child->setParent(shared_from_this());
        m_children.push_back(std::move(child));
    }

    std::vector<Node*> Node::getChildren() const
    {
        std::vector<Node*> raw_children;
        for (const auto& child : m_children)
        {
            raw_children.push_back(child.get());
        }
        return raw_children;
    }

    bool Node::removeChild(const std::string& label)
    {
        auto it = std::remove_if(m_children.begin(), m_children.end(),
                                [&label](const std::unique_ptr<Node>& child) 
                                 { return child->getLabel() == label; });
        
        if (it != m_children.end())
        {
            m_children.erase(it, m_children.end());
            return true;
        }
        return false;
    }

    void Node::UpdatePath()
    {
        if (auto p = m_parent.lock())
        {
            m_path = p->getPath() + "/" + m_label;
        }
        else
        {
            m_path = m_label;
        }

        for (auto& child : m_children)
        {
            child->UpdatePath();
        }
    }


} // namespace cgx::scene