// Copyright © 2024 Jacob Curlin

#pragma once

#include "cgx.h"

class Sandbox final : public cgx::core::Engine
{
public:
    Sandbox();
    ~Sandbox() override;

protected:
    void initialize() override;
    void update() override;
    void render() override;

    void geometry_test() const;

    // void audio_setup();
    //void audio_test();

    //ALCdevice* m_al_device{nullptr};
    //ALCcontext* m_al_context{nullptr};
    //ALuint m_al_buffer{0};
    //ALuint m_al_source{0};

    void load_assets() const;
};
