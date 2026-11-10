#include "Game.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

Game::Game()
    : m_window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game (SFML C++)", sf::Style::Titlebar | sf::Style::Close),
      m_state(GameState::Start),
      m_score(0),
      m_highScore(0),
      m_level(1),
      m_accumulator(0.0f),
      m_stepTime(BASE_STEP_TIME),
      m_startBlinkTimer(0.0f) {
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);

    loadHighScore();
    resetGame();
    m_state = GameState::Start;
}

void Game::resetGame() {
    m_score = 0;
    m_level = 1;
    m_stepTime = BASE_STEP_TIME;
    m_accumulator = 0.0f;

    // Reset snake in the center of the playable field
    sf::Vector2i startPos(GRID_WIDTH / 2, (PLAY_MIN_Y + PLAY_MAX_Y) / 2);
    m_snake.reset(startPos);

    // Spawn initial food item
    m_food.respawn(m_snake.getBody(), PLAY_MIN_X, PLAY_MAX_X, PLAY_MIN_Y, PLAY_MAX_Y);
}

void Game::run() {
    m_clock.restart();

    while (m_window.isOpen()) {
        float dt = m_clock.restart().asSeconds();
        // Prevent large accumulator jumps if the window is moved or briefly frozen
        if (dt > 0.2f) {
            dt = 0.2f;
        }

        processEvents();

        if (m_state == GameState::Playing) {
            m_accumulator += dt;
            while (m_accumulator >= m_stepTime) {
                updateFixedStep();
                m_accumulator -= m_stepTime;
            }
        } else {
            m_accumulator = 0.0f;
        }

        update(dt);

        float alpha = (m_state == GameState::Playing && m_stepTime > 0.0f)
                      ? (m_accumulator / m_stepTime)
                      : 1.0f;

        render(alpha);
    }
}

void Game::processEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        } else if (event.type == sf::Event::KeyPressed) {
            handleInput(event);
        }
    }
}

void Game::handleInput(const sf::Event& event) {
    sf::Keyboard::Key key = event.key.code;

    // Global Quit
    if (key == sf::Keyboard::Escape) {
        m_window.close();
        return;
    }

    switch (m_state) {
        case GameState::Start: {
            if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) {
                m_state = GameState::Playing;
                m_audio.play(SoundId::Start);
            }
            break;
        }
        case GameState::Playing: {
            // Movement Controls (WASD & Arrow Keys)
            if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
                m_snake.queueDirection(Direction::Up);
            } else if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
                m_snake.queueDirection(Direction::Down);
            } else if (key == sf::Keyboard::A || key == sf::Keyboard::Left) {
                m_snake.queueDirection(Direction::Left);
            } else if (key == sf::Keyboard::D || key == sf::Keyboard::Right) {
                m_snake.queueDirection(Direction::Right);
            } else if (key == sf::Keyboard::Space) {
                m_state = GameState::Paused;
                m_audio.play(SoundId::Pause);
            }
            break;
        }
        case GameState::Paused: {
            if (key == sf::Keyboard::Space) {
                m_state = GameState::Playing;
                m_audio.play(SoundId::Pause);
            }
            break;
        }
        case GameState::GameOver:
        case GameState::Win: {
            if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) {
                resetGame();
                m_state = GameState::Playing;
                m_audio.play(SoundId::Start);
            }
            break;
        }
    }
}

void Game::updateFixedStep() {
    sf::Vector2i nextHead = m_snake.getNextHeadPosition();
    bool willGrow = (nextHead == m_food.getPosition());

    // 1. Check Wall Collision
    if (nextHead.x < PLAY_MIN_X || nextHead.x > PLAY_MAX_X ||
        nextHead.y < PLAY_MIN_Y || nextHead.y > PLAY_MAX_Y) {
        triggerGameOver();
        return;
    }

    // 2. Check Self Collision (properly accounting for tail vacation)
    if (m_snake.checkSelfCollision(nextHead, willGrow)) {
        triggerGameOver();
        return;
    }

    // 3. Advance snake
    m_snake.step(willGrow);

    // 4. Handle Food Consumption
    if (willGrow) {
        m_score += 10;
        if (m_score > m_highScore) {
            m_highScore = m_score;
            saveHighScore();
        }

        // Increase level and speed every 50 points
        m_level = 1 + (m_score / 50);
        m_stepTime = std::max(MIN_STEP_TIME, BASE_STEP_TIME - static_cast<float>(m_level - 1) * 0.007f);

        m_audio.play(SoundId::Eat);

        // Respawn food on an available empty cell
        bool spawned = m_food.respawn(m_snake.getBody(), PLAY_MIN_X, PLAY_MAX_X, PLAY_MIN_Y, PLAY_MAX_Y);
        if (!spawned) {
            triggerWin();
        }
    }
}

void Game::update(float dt) {
    m_food.updateAnimation(dt);

    if (m_state == GameState::Start) {
        m_startBlinkTimer += dt;
    }
}

void Game::render(float alpha) {
    m_window.clear(sf::Color(18, 20, 28));

    // 1. Render arena grid and playing field
    m_ui.renderGrid(m_window, PLAY_MIN_X, PLAY_MAX_X, PLAY_MIN_Y, PLAY_MAX_Y, CELL_SIZE);

    // 2. Render Food
    m_food.render(m_window, CELL_SIZE);

    // 3. Render Snake with smooth visual interpolation
    m_snake.render(m_window, alpha, CELL_SIZE);

    // 4. Render top HUD
    m_ui.renderHUD(m_window, m_score, m_highScore, m_level, WINDOW_WIDTH, HUD_HEIGHT);

    // 5. Render overlays based on game state
    switch (m_state) {
        case GameState::Start:
            m_ui.renderStartScreen(m_window, WINDOW_WIDTH, WINDOW_HEIGHT, m_startBlinkTimer);
            break;
        case GameState::Paused:
            m_ui.renderPauseScreen(m_window, WINDOW_WIDTH, WINDOW_HEIGHT);
            break;
        case GameState::GameOver:
            m_ui.renderGameOverScreen(m_window, m_score, m_highScore, WINDOW_WIDTH, WINDOW_HEIGHT);
            break;
        case GameState::Win:
            m_ui.renderWinScreen(m_window, m_score, WINDOW_WIDTH, WINDOW_HEIGHT);
            break;
        case GameState::Playing:
            break;
    }

    m_window.display();
}

void Game::triggerGameOver() {
    m_state = GameState::GameOver;
    m_audio.play(SoundId::GameOver);
    if (m_score > m_highScore) {
        m_highScore = m_score;
        saveHighScore();
    }
}

void Game::triggerWin() {
    m_state = GameState::Win;
    m_audio.play(SoundId::Start);
    if (m_score > m_highScore) {
        m_highScore = m_score;
        saveHighScore();
    }
}

void Game::loadHighScore() {
    std::ifstream inFile(m_highScoreFile);
    if (inFile.is_open()) {
        inFile >> m_highScore;
        if (inFile.fail()) {
            m_highScore = 0;
        }
        inFile.close();
    } else {
        m_highScore = 0;
    }
}

void Game::saveHighScore() {
    std::ofstream outFile(m_highScoreFile);
    if (outFile.is_open()) {
        outFile << m_highScore << "\n";
        outFile.close();
    }
}

