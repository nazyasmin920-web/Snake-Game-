#pragma once

#include <SFML/Graphics.hpp>
#include "Snake.hpp"
#include "Food.hpp"
#include "UI.hpp"
#include "Audio.hpp"
#include "Obstacle.hpp"
#include <string>
#include <vector>

enum class GameState {
    ModeSelect,   // NEW: main menu to pick mode
    Start,
    Playing,
    Paused,
    Dying,
    GameOver,
    Win
};

enum class GameMode {
    Classic,      // original endless game
    Challenge     // eat N foods before countdown hits zero
};

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
    float maxLifetime;
    float size;
};

struct FloatingText {
    std::string text;
    sf::Vector2f position;
    sf::Color color;
    float lifetime;
    float maxLifetime;
};

class Game {
public:
    Game();
    ~Game() = default;

    void run();

private:
    void processEvents();
    void handleInput(const sf::Event& event);
    void update(float dt);
    void updateFixedStep();
    void render(float alpha);

    void resetGame();
    void startDeathSequence();
    void triggerGameOver();
    void triggerWin();

    void spawnFoodParticles(const sf::Vector2f& pos);
    void spawnDeathParticles();
    void spawnLevelUpParticles(const sf::Vector2f& pos);

    void loadHighScore();
    void saveHighScore();

    static constexpr int CELL_SIZE = 20;
    static constexpr int GRID_WIDTH = 40;
    static constexpr int GRID_HEIGHT = 30;
    static constexpr int WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE;   // 800
    static constexpr int WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE; // 600
    static constexpr int HUD_HEIGHT = 40;
    static constexpr int PLAY_MIN_X = 0;
    static constexpr int PLAY_MAX_X = GRID_WIDTH - 1;            // 39
    static constexpr int PLAY_MIN_Y = 2;
    static constexpr int PLAY_MAX_Y = GRID_HEIGHT - 1;           // 29

    static constexpr float BASE_STEP_TIME = 0.12f;
    static constexpr float MIN_STEP_TIME = 0.05f;

    sf::RenderWindow m_window;
    sf::View m_defaultView;

    Snake m_snake;
    Food m_food;
    UI m_ui;
    Audio m_audio;
    ObstacleManager m_obstacles;

    GameState m_state;
    GameMode  m_gameMode;
    int m_score;
    float m_displayedScore;
    int m_highScore;
    int m_level;
    float m_gameTime;

    // ── Challenge mode vars ──────────────────────────
    int   m_challengeWave;        // current wave (1-based)
    int   m_challengeTarget;      // foods to eat this wave
    int   m_challengeFoodEaten;   // foods eaten so far this wave
    float m_challengeTimeLeft;    // countdown seconds remaining
    float m_challengeWaveFlash;   // brief "WAVE COMPLETE" flash timer

    sf::Clock m_clock;
    float m_accumulator;
    float m_stepTime;
    float m_startBlinkTimer;

    // Visual polish systems
    float m_screenShake;
    float m_deathDuration;
    std::vector<Particle> m_particles;
    std::vector<FloatingText> m_floatingTexts;

    const std::string m_highScoreFile = "highscore.txt";
};
