// Copyright © 2024 Jacob Curlin

#include "render/render_system.h"

#include "render/camera.h"
#include "render/framebuffer.h"
#include "resource/model.h"
#include "resource/shader.h"
#include "scene/scene.h"
#include "utility/error.h"

#include "ecs/component_manager.h"
#include "ecs/components/transform.h"
#include "ecs/components/render.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "GLFW/glfw3.h"

namespace cgx::render
{
    RenderSystem::RenderSystem(std::shared_ptr<cgx::ecs::ComponentManager> component_registry) 
        : System(component_registry)
    {}

    void RenderSystem::Initialize()
    {
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            CGX_CRITICAL("Failed ot initialize GLAD.");
            exit(1);
        }
        glEnable(GL_DEPTH_TEST); CGX_CHECK_GL_ERROR;

        m_camera = std::make_unique<cgx::render::Camera>();

        m_framebuffer = std::make_shared<cgx::render::Framebuffer>(m_render_width, m_render_height);
        m_framebuffer->setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    }

    void RenderSystem::Update(float dt)
    {
        m_camera->Update(dt);
    }

    void RenderSystem::Render()
    {
        float r, g, b, a;
        m_framebuffer->getClearColor(r, g, b, a);
        glDisable(GL_CULL_FACE);
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_msaa_enabled)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_msaa_framebuffer);
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer->getFBO());
        }

        glViewport(0, 0, m_render_width, m_render_height);
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 view_mat = m_camera->getViewMatrix();
        glm::mat4 proj_mat = glm::perspective(
                                glm::radians(m_camera->getZoom()),
                                (float) m_render_height / (float) m_render_width,
                                0.1f, 100.0f);

        for (auto& entity : m_entities)
        {
            glm::mat4 model_mat;

            auto& render_c = GetComponent<cgx::component::Render>(entity);
            auto& transform_c = GetComponent<cgx::component::Transform>(entity);

            if (!(render_c.model && render_c.shader)) { continue; }

            // apply rotations transformations around each axis
            model_mat = glm::rotate(model_mat, glm::radians(transform_c.local_rotation.x),
                                    glm::vec3(1.0f, 0.0f, 0.0f)); // X-axis
            model_mat = glm::rotate(model_mat, glm::radians(transform_c.local_rotation.y),
                                    glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis
            model_mat = glm::rotate(model_mat, glm::radians(transform_c.local_rotation.z),
                                    glm::vec3(0.0f, 0.0f, 1.0f)); // Z-axis

            model_mat = glm::translate(model_mat, transform_c.local_position);  // apply position transformation

            model_mat = glm::scale(model_mat, transform_c.local_scale); // apply scale transformation

            // activate shader program, set shader data, draw
            render_c.shader->Use();

            // shader->setVec3("light.position", (m_ecsHandler->GetComponent<TransformComponent>(light))) /TODO
            render_c.shader->setVec3("light.position", 1.0f, 1.0f, 1.0f);
            render_c.shader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
            render_c.shader->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
            render_c.shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

            render_c.shader->setMat4("proj", proj_mat);
            render_c.shader->setMat4("view", view_mat);
            render_c.shader->setMat4("model", model_mat);

            render_c.model->Draw(*(render_c.shader));
        }

        // if (m_render_settings->skybox) { SkyboxRender(); }

        if (m_msaa_enabled)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaa_framebuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer->getFBO());
            glBlitFramebuffer(0, 0, m_render_width, m_render_height,
                            0, 0, m_render_width, m_render_height, 
                            GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, m_window_width, m_window_height);

    }

    void RenderSystem::InitializeMSAA()
    {
        // setup MSAA frame buffer object
        glGenFramebuffers(1, &m_msaa_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_msaa_framebuffer);

        unsigned int msaa_tex_color_buf;
        glGenTextures(1, &msaa_tex_color_buf); CGX_CHECK_GL_ERROR;
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaa_tex_color_buf); CGX_CHECK_GL_ERROR;
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, m_render_width, m_render_height, GL_TRUE); CGX_CHECK_GL_ERROR;
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0); CGX_CHECK_GL_ERROR;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaa_tex_color_buf, 0); CGX_CHECK_GL_ERROR;

        unsigned int msaa_rbo;
        glGenRenderbuffers(1, &msaa_rbo); CGX_CHECK_GL_ERROR;
        glBindRenderbuffer(GL_RENDERBUFFER, msaa_rbo); CGX_CHECK_GL_ERROR;
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, m_render_width, m_render_height); CGX_CHECK_GL_ERROR;
        glBindRenderbuffer(GL_RENDERBUFFER, 0); CGX_CHECK_GL_ERROR;
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaa_rbo); CGX_CHECK_GL_ERROR;

        CGX_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "MSAA Framebuffer not complete."); 
        glBindFramebuffer(GL_FRAMEBUFFER, 0); CGX_CHECK_GL_ERROR;
    }

} // namespace cgx::render
