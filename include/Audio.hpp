#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <array>

enum class SoundId {
    Eat,
    GameOver,
    Start,
    Pause,
    Count
};

class Audio {
public:
    Audio();

    // Initialize audio buffers from files or procedural synthesis
    void initialize();

    // Play a sound effect by ID (fails silently and safely if audio is unavailable)
    void play(SoundId id);

    void setMuted(bool muted);
    bool isMuted() const;

private:
    void generateProceduralBuffer(SoundId id);

    std::array<sf::SoundBuffer, static_cast<size_t>(SoundId::Count)> m_buffers;
    std::array<sf::Sound, static_cast<size_t>(SoundId::Count)> m_sounds;
    bool m_audioAvailable;
    bool m_muted;
};

