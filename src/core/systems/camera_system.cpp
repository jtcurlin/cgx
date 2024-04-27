// Copyright © 2024 Jacob Curlin

#include "core/systems/camera_system.h"

#include "core/components/camera.h"
#include "core/components/transform.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace cgx::core
{
CameraSystem::CameraSystem(ecs::ECSManager* ecs_manager)
    : System(ecs_manager) {}

CameraSystem::~CameraSystem() = default;

void CameraSystem::update(float dt)
{
    for (const auto& entity : m_entities) {
        auto& camera    = get_component<component::Camera>(entity);
        auto& transform = get_component<component::Transform>(entity);

        auto view = glm::mat4(1.0f);

        // compute the view matrix based on the transform component
        view = glm::rotate(view, glm::radians(-transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(-transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, glm::radians(-transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::translate(view, -transform.translation);

        // set the computed view matrix in the camera component
        camera.view_matrix = view;
    }
}

void CameraSystem::on_entity_added(ecs::Entity entity) {}
void CameraSystem::on_entity_removed(ecs::Entity entity) {}
}

/*
void CameraSystem::update(float dt)
{
    for (const auto& entity : m_entities) {
        auto& camera = get_component<component::Camera>(entity);
        auto& transform = get_component<component::Transform>(entity);

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform.translation);
        glm::mat4 rotation = glm::toMat4(glm::quat(transform.rotation));
        camera.view_matrix = glm::inverse(translation * rotation);
    }

}
*/
