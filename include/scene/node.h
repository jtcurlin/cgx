// Copyright © 2024 Jacob Curlin

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace cgx::scene
{
    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        Node(std::string label);
        virtual ~Node() = default;

        std::string getLabel() const;
        void setLabel(const std::string& label);

        const std::string& getPath() const;

        std::shared_ptr<Node> getParent() const;
        void setParent(const std::shared_ptr<Node>& parent);

        void addChild(std::unique_ptr<Node> child);
        bool removeChild(const std::string& label);

        std::vector<Node*> getChildren() const;

    protected:
        std::string m_label;
        std::string m_path;

        std::weak_ptr<Node>                m_parent{};
        std::vector<std::unique_ptr<Node>> m_children{};

        void UpdatePath();

    }; // class SceneNode

} // namespace cgx::ecs

