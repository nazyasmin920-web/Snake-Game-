#pragma once

#include <SFML/Graphics.hpp>
#include "Snake.hpp"
#include "Food.hpp"
#include "UI.hpp"
#include "Audio.hpp"
#include <string>

enum class GameState {
    Start,
    Playing,
    Paused,
    GameOver,
    Win
};

class Game {
public:
    Game();
    ~Game() = default;

    // Run the main game loop
    void run();

private:
    // Core game loop stages
    void processEvents();
    void handleInput(const sf::Event& event);
    void update(float dt);
    void updateFixedStep();
    void render(float alpha);

    // Game lifecycle helpers
    void resetGame();
    void triggerGameOver();
    void triggerWin();

    // High score persistence
    void loadHighScore();
    void saveHighScore();

    // Game constants
    static constexpr int CELL_SIZE = 20;
    static constexpr int GRID_WIDTH = 40;
    static constexpr int GRID_HEIGHT = 30;
    static constexpr int WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE;   // 800
    static constexpr int WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE; // 600
    static constexpr int HUD_HEIGHT = 40;
    static constexpr int PLAY_MIN_X = 0;
    static constexpr int PLAY_MAX_X = GRID_WIDTH - 1;            // 39
    static constexpr int PLAY_MIN_Y = 2;                         // Rows 0 and 1 are reserved for HUD
    static constexpr int PLAY_MAX_Y = GRID_HEIGHT - 1;           // 29

    static constexpr float BASE_STEP_TIME = 0.12f;
    static constexpr float MIN_STEP_TIME = 0.05f;

    // SFML Window
    sf::RenderWindow m_window;

    // Game sub-systems
    Snake m_snake;
    Food m_food;
    UI m_ui;
    Audio m_audio;

    // State management
    GameState m_state;
    int m_score;
    int m_highScore;
    int m_level;

    // Timing
    sf::Clock m_clock;
    float m_accumulator;
    float m_stepTime;
    float m_startBlinkTimer;

    // High score file path
    const std::string m_highScoreFile = "highscore.txt";
};

