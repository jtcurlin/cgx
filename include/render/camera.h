// Copyright © 2024 Jacob Curlin
// source: (LearnOpenGL) https://learnopengl.com/Getting-started/Camera

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cgx::render
{
enum TranslateDirection
{
    kForward,
    kBackward,
    kLeft,
    kRight
};

constexpr float kYaw              = -90.0f;
constexpr float kPitch            = 0.0f;
constexpr float kMovementSpeed    = 2.5f;
constexpr float kMouseSensitivity = 0.1f;
constexpr float kZoom             = 45.0f;

class Camera
{
public:
    explicit Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
        float     yaw      = kYaw,
        float     pitch    = kPitch);
    ~Camera() = default ;

    void Update(double dt);

    glm::mat4 getViewMatrix() const;

    [[nodiscard]] float getZoom() const;

    void Translate(TranslateDirection dir, double dt);
    void Look(double x_offset, double y_offset, GLboolean constrain_pitch);

    void EnableManualControl();
    void DisableManualControl();

private:
    void updateCameraVectors();

    bool m_manual_control_enabled = false;

    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_front    = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_up       = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_right    = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_world_up = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_yaw               = kYaw;
    float m_pitch             = kPitch;
    float m_movement_speed    = kMovementSpeed;
    float m_mouse_sensitivity = kMouseSensitivity;
    float m_zoom              = kZoom;

    float near_clip = 0.1f;
    float far_clip  = 100.0f;
};
}
