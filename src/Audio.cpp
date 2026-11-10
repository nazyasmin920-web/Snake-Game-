#include "Audio.hpp"
#include <iostream>
#include <vector>
#include <cmath>

Audio::Audio()
    : m_audioAvailable(false),
      m_muted(false) {
    initialize();
}

void Audio::initialize() {
    try {
        const std::pair<SoundId, std::string> soundFiles[] = {
            {SoundId::Eat, "assets/sounds/eat.wav"},
            {SoundId::GameOver, "assets/sounds/gameover.wav"},
            {SoundId::Start, "assets/sounds/start.wav"},
            {SoundId::Pause, "assets/sounds/pause.wav"}
        };

        for (const auto& item : soundFiles) {
            size_t idx = static_cast<size_t>(item.first);
            bool loaded = false;

            const std::string candidatePaths[] = {
                item.second,
                "../" + item.second,
                "../../" + item.second
            };

            for (const auto& path : candidatePaths) {
                if (m_buffers[idx].loadFromFile(path)) {
                    loaded = true;
                    break;
                }
            }

            if (!loaded) {
                generateProceduralBuffer(item.first);
            }

            m_sounds[idx].setBuffer(m_buffers[idx]);
        }

        m_audioAvailable = true;
    } catch (const std::exception& ex) {
        std::cerr << "[Audio] Sound system disabled: " << ex.what() << std::endl;
        m_audioAvailable = false;
    } catch (...) {
        std::cerr << "[Audio] Sound system disabled due to unknown audio device error." << std::endl;
        m_audioAvailable = false;
    }
}

void Audio::generateProceduralBuffer(SoundId id) {
    const unsigned int sampleRate = 44100;
    const double pi = 3.14159265358979323846;
    std::vector<sf::Int16> samples;

    switch (id) {
        case SoundId::Eat: {
            double duration = 0.08;
            size_t numSamples = static_cast<size_t>(sampleRate * duration);
            samples.resize(numSamples);
            for (size_t i = 0; i < numSamples; ++i) {
                double t = static_cast<double>(i) / sampleRate;
                double progress = t / duration;
                double freq = 550.0 + (450.0 * progress);
                double envelope = std::pow(1.0 - progress, 2.0);
                double wave = std::sin(2.0 * pi * freq * t);
                samples[i] = static_cast<sf::Int16>(wave * envelope * 24000.0);
            }
            break;
        }
        case SoundId::GameOver: {
            double duration = 0.35;
            size_t numSamples = static_cast<size_t>(sampleRate * duration);
            samples.resize(numSamples);
            for (size_t i = 0; i < numSamples; ++i) {
                double t = static_cast<double>(i) / sampleRate;
                double progress = t / duration;
                double freq = 300.0 - (180.0 * progress);
                double envelope = 1.0 - progress;
                double wave = std::sin(2.0 * pi * freq * t) + 0.3 * std::sin(4.0 * pi * freq * t);
                samples[i] = static_cast<sf::Int16>(wave * envelope * 22000.0);
            }
            break;
        }
        case SoundId::Start: {
            double duration = 0.22;
            size_t numSamples = static_cast<size_t>(sampleRate * duration);
            samples.resize(numSamples);
            for (size_t i = 0; i < numSamples; ++i) {
                double t = static_cast<double>(i) / sampleRate;
                double freq = (t < 0.11) ? 523.25 : 659.25;
                double localT = (t < 0.11) ? t : (t - 0.11);
                double envelope = std::exp(-localT * 12.0);
                double wave = std::sin(2.0 * pi * freq * t);
                samples[i] = static_cast<sf::Int16>(wave * envelope * 22000.0);
            }
            break;
        }
        case SoundId::Pause: {
            double duration = 0.05;
            size_t numSamples = static_cast<size_t>(sampleRate * duration);
            samples.resize(numSamples);
            for (size_t i = 0; i < numSamples; ++i) {
                double t = static_cast<double>(i) / sampleRate;
                double freq = 650.0;
                double envelope = std::exp(-t * 60.0);
                double wave = std::sin(2.0 * pi * freq * t);
                samples[i] = static_cast<sf::Int16>(wave * envelope * 20000.0);
            }
            break;
        }
        default:
            return;
    }

    size_t idx = static_cast<size_t>(id);
    m_buffers[idx].loadFromSamples(samples.data(), samples.size(), 1, sampleRate);
}

void Audio::play(SoundId id) {
    if (!m_audioAvailable || m_muted) return;

    size_t idx = static_cast<size_t>(id);
    if (idx < m_sounds.size()) {
        try {
            m_sounds[idx].play();
        } catch (...) {
            // Silently suppress playback errors
        }
    }
}

void Audio::setMuted(bool muted) {
    m_muted = muted;
}

bool Audio::isMuted() const {
    return m_muted;
}

