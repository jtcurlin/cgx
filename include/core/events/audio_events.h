// Copyright © 2024 Jacob Curlin

// path: core/events/audio_events.h

#pragma once

#include <glm/vec3.hpp>

#include "core/event.h" // this defines the Event object
#include "scene/node.h"

namespace cgx::component {
struct Transform;
}

namespace cgx::ecs
{
class ECSManager;
}

namespace cgx::core::event::audio
{

// this is the event id of the event itself
constexpr EventId PLAY_SOUND = "event::audio::PLAY_SOUND"_hash;

// this is the entity from which the sound originates
constexpr ParamId ENTITY_ID = "event::audio::SOURCE_POSITION"_hash;

// this is an int/uint identifier for the sound to be played
constexpr ParamId SOUND_ID = "event::audio::SOUND_ID"_hash;
}

#include "core/event_handler.h"


namespace cgx::gui
{
    // within audio component menu in gui/panels/properties_panel.cpp

    // when a button is pressed or something, we can do this to send the event to play the sound
    cgx::audio::SoundId sound_id = audio::AudioManager()::get_singleton().get_sound_id_from_tag("Star Wars");
    glm::vec3 position = m_ecs_manager<component::Transform>(node->get_entity()).translation;

    core::event::Event play_sound_event(core::event::audio::PLAY_SOUND);
    play_sound_event.set_param<glm::vec3>(core::event::audio::SOUND_POSITION, position);
    play_sound_event.set_param<cgx::audio::size_t>(core::event::audio::SOUND_ID, sound_id);
    core::EventHandler::get_instance().send_event(play_sound_event);

}

namespace cgx::asset
{
class Sound;
}

namespace cgx::audio
{

using SoundId = uint32_t;

class AudioManager
{
public:
    AudioManager(ecs::ECSManager* ecs_manager)
        : m_ecs_manager(ecs_manager) {}

    // we'll have to call this method somewhere in engine initialization to setup the events
    // that the audio manager needs to listen for
    void setup_audio_events()
    {
        // get the event handlder instance since its a singleton, and use the .add_listener method
        core::EventHandler::get_instance().add_listener(
            core::event::audio::PLAY_SOUND,
            [this](core::event::Event& event) {

                // what you want to happen when the sound plays

                // get the vec3 position in world space where the sound was triggered from
                auto source_position = event.get_param<glm::vec3>(core::event::audio::SOURCE_POSITION);

                // get the sound id of the sound to be played
                auto sound_to_play_id = event.get_param<SoundId>(core::event::audio::SOUND_ID);

                // execute the AudioManager::PlaySound command
                this->process_event(sound_to_play_id, source_position);
            });
    }

    void process_event(SoundId sound_id, glm::vec3 source_position)
    {
        auto sound_it = m_sounds.find(sound_id); // check sound dictionary for mapping for our sound id
        if (sound_it != m_sounds.end())          // if id present in our mapping
        {
            asset::Sound* curr_sound = sound_it->second; // get the pointer to the sound asset itself (iterator->second)

            // you now have a pointer your sound asset ('curr_sound')
            //  and a 3d vector representing the position it originated from ('source_position')

            // >> now you can do whatever
            // you can play the sound here, you can enque the actual Sound* asset with the position, etc.
        }
        else {
            return; // if we didn't find a sound associated with the provided sound id in our m_sounds map, no action
        }
    }

    SoundId get_sound_id_from_tag(std::string sound_tag);

private:
    std::unordered_map<SoundId, asset::Sound*>  m_sounds;
    ecs::ECSManager*                            m_ecs_manager;
    std::queue<std::pair<size_t, glm::vec3>>    m_sound_queue;
};
}
