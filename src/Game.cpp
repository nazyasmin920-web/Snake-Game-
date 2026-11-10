#include "Game.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

Game::Game()
    : m_window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game Pro (SFML C++)", sf::Style::Titlebar | sf::Style::Close),
      m_state(GameState::Start),
      m_score(0),
      m_displayedScore(0.0f),
      m_highScore(0),
      m_level(1),
      m_gameTime(0.0f),
      m_accumulator(0.0f),
      m_stepTime(BASE_STEP_TIME),
      m_startBlinkTimer(0.0f),
      m_screenShake(0.0f),
      m_deathDuration(0.0f) {
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);
    m_defaultView = m_window.getDefaultView();

    loadHighScore();
    resetGame();
    m_state = GameState::Start;
}

void Game::resetGame() {
    m_score = 0;
    m_displayedScore = 0.0f;
    m_level = 1;
    m_gameTime = 0.0f;
    m_stepTime = BASE_STEP_TIME;
    m_accumulator = 0.0f;
    m_screenShake = 0.0f;
    m_deathDuration = 0.0f;
    m_particles.clear();
    m_floatingTexts.clear();

    sf::Vector2i startPos(GRID_WIDTH / 2, (PLAY_MIN_Y + PLAY_MAX_Y) / 2);
    m_snake.reset(startPos);
    m_snake.setLevel(1);
    m_obstacles.clear();
    m_food.respawn(m_snake.getBody(), m_obstacles.getObstacles(), PLAY_MIN_X, PLAY_MAX_X, PLAY_MIN_Y, PLAY_MAX_Y);
}

void Game::run() {
    m_clock.restart();

    while (m_window.isOpen()) {
        float dt = m_clock.restart().asSeconds();
        if (dt > 0.2f) dt = 0.2f;

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
        case GameState::Dying: {
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
    // 1. Get next head position with boundary wrap (no wall death)
    sf::Vector2i nextHead = m_snake.getNextHeadPosition(PLAY_MIN_X, PLAY_MAX_X, PLAY_MIN_Y, PLAY_MAX_Y);
    bool willGrow = (nextHead == m_food.getPosition());

    // 2. Self Collision: Snake only dies if it bites into its own body!
    if (m_snake.checkSelfCollision(nextHead, willGrow)) {
        startDeathSequence();
        return;
    }

    // 2b. Obstacle Collision
    if (m_obstacles.collidesWith(nextHead)) {
        startDeathSequence();
        return;
    }

    // 3. Advance snake
    m_snake.step(PLAY_MIN_X, PLAY_MAX_X, PLAY_MIN_Y, PLAY_MAX_Y, willGrow);

    // 4. Handle Food Consumption
    if (willGrow) {
        m_score += 10;
        if (m_score > m_highScore) {
            m_highScore = m_score;
            saveHighScore();
        }

        // Level increases every 5 food items collected (every 50 points)
        int newLevel = 1 + (m_score / 50);
        if (newLevel > m_level) {
            m_level = newLevel;
            m_snake.setLevel(m_level); // Transforms the snake's color!

            // Celebratory Level-Up Sound & Banner
            m_audio.play(SoundId::Start);

            sf::Vector2f headPx(
                (static_cast<float>(nextHead.x) + 0.5f) * CELL_SIZE,
                (static_cast<float>(nextHead.y) + 0.5f) * CELL_SIZE
            );
            spawnLevelUpParticles(headPx);

            FloatingText ftLvl;
            if (m_level == 5) {
                ftLvl.text = "LEVEL 5! CRIMSON DRAGON FORM!";
                ftLvl.color = sf::Color(255, 45, 75);
            } else {
                ftLvl.text = "LEVEL UP! COLOR UNLOCKED!";
                ftLvl.color = sf::Color(241, 196, 15);
            }
            ftLvl.position = sf::Vector2f(WINDOW_WIDTH / 2.0f, 180.0f);
            ftLvl.lifetime = 1.6f;
            ftLvl.maxLifetime = 1.6f;
            m_floatingTexts.push_back(ftLvl);
        }

        m_stepTime = std::max(MIN_STEP_TIME, BASE_STEP_TIME - static_cast<float>(m_level - 1) * 0.007f);

        m_audio.play(SoundId::Eat);
        m_snake.triggerDigestBulge();

        sf::Vector2f foodPx = m_food.getPixelPosition(CELL_SIZE);
        spawnFoodParticles(foodPx);

        FloatingText ft;
        ft.text = "+10";
        ft.position = foodPx;
        ft.color = sf::Color(241, 196, 15);
        ft.lifetime = 0.8f;
        ft.maxLifetime = 0.8f;
        m_floatingTexts.push_back(ft);

        bool spawned = m_food.respawn(m_snake.getBody(), m_obstacles.getObstacles(), PLAY_MIN_X, PLAY_MAX_X, PLAY_MIN_Y, PLAY_MAX_Y);
        if (!spawned) {
            triggerWin();
        } else if (m_level >= 6) {
            // Spawn obstacles near the new food position, every 6 levels
            m_obstacles.onFoodSpawned(m_food.getPosition(), m_snake.getBody(),
                                      PLAY_MIN_X, PLAY_MAX_X, PLAY_MIN_Y, PLAY_MAX_Y, m_level);
        }
    }
}

void Game::startDeathSequence() {
    m_state = GameState::Dying;
    m_deathDuration = 1.3f;
    m_screenShake = 12.0f;
    m_snake.setDead(true);

    spawnDeathParticles();
    m_audio.play(SoundId::GameOver);
}

void Game::spawnFoodParticles(const sf::Vector2f& pos) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> angleDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> speedDist(40.0f, 130.0f);
    std::uniform_real_distribution<float> sizeDist(2.0f, 4.0f);
    std::uniform_real_distribution<float> lifeDist(0.35f, 0.65f);

    for (int i = 0; i < 18; ++i) {
        float angle = angleDist(rng);
        float speed = speedDist(rng);
        Particle p;
        p.position = pos;
        p.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        p.size = sizeDist(rng);
        p.lifetime = lifeDist(rng);
        p.maxLifetime = p.lifetime;

        if (i % 2 == 0) {
            p.color = sf::Color(255, 215, 0);
        } else {
            p.color = sf::Color(255, 75, 95);
        }
        m_particles.push_back(p);
    }
}

void Game::spawnLevelUpParticles(const sf::Vector2f& pos) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> angleDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> speedDist(90.0f, 260.0f);
    std::uniform_real_distribution<float> sizeDist(3.0f, 6.0f);
    std::uniform_real_distribution<float> lifeDist(0.8f, 1.4f);

    for (int i = 0; i < 35; ++i) {
        float angle = angleDist(rng);
        float speed = speedDist(rng);
        Particle p;
        p.position = pos;
        p.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        p.size = sizeDist(rng);
        p.lifetime = lifeDist(rng);
        p.maxLifetime = p.lifetime;

        if (i % 3 == 0) {
            p.color = sf::Color(255, 45, 75); // Ruby
        } else if (i % 3 == 1) {
            p.color = sf::Color(0, 229, 255); // Cyan
        } else {
            p.color = sf::Color(255, 215, 0); // Gold
        }
        m_particles.push_back(p);
    }
}

void Game::spawnDeathParticles() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> angleDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> speedDist(60.0f, 220.0f);
    std::uniform_real_distribution<float> sizeDist(2.5f, 5.0f);
    std::uniform_real_distribution<float> lifeDist(0.5f, 1.1f);

    const auto& body = m_snake.getBody();
    for (size_t s = 0; s < body.size(); ++s) {
        sf::Vector2f segPos(
            (static_cast<float>(body[s].x) + 0.5f) * CELL_SIZE,
            (static_cast<float>(body[s].y) + 0.5f) * CELL_SIZE
        );

        int count = (s == 0) ? 14 : 5;
        for (int i = 0; i < count; ++i) {
            float angle = angleDist(rng);
            float speed = speedDist(rng);
            Particle p;
            p.position = segPos;
            p.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
            p.size = sizeDist(rng);
            p.lifetime = lifeDist(rng);
            p.maxLifetime = p.lifetime;

            if (i % 3 == 0) {
                p.color = sf::Color(231, 76, 60);
            } else if (i % 3 == 1) {
                p.color = sf::Color(46, 204, 113);
            } else {
                p.color = sf::Color(241, 196, 15);
            }
            m_particles.push_back(p);
        }
    }
}

void Game::update(float dt) {
    m_food.updateAnimation(dt);
    m_snake.updateAnimation(dt);
    m_obstacles.updateAnimation(dt);

    if (m_state == GameState::Start) {
        m_startBlinkTimer += dt;
    }

    // Active game timer: advances only while playing
    if (m_state == GameState::Playing) {
        m_gameTime += dt;
    }

    if (m_state == GameState::Dying) {
        m_deathDuration -= dt;
        if (m_deathDuration <= 0.0f) {
            triggerGameOver();
        }
    }

    if (m_screenShake > 0.0f) {
        m_screenShake = std::max(0.0f, m_screenShake - dt * 26.0f);
    }

    m_displayedScore += (static_cast<float>(m_score) - m_displayedScore) * std::min(1.0f, dt * 10.0f);

    for (auto& p : m_particles) {
        p.position += p.velocity * dt;
        p.velocity *= 0.92f;
        p.lifetime -= dt;
    }
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
                       [](const Particle& p) { return p.lifetime <= 0.0f; }),
        m_particles.end()
    );

    for (auto& ft : m_floatingTexts) {
        ft.position.y -= 30.0f * dt;
        ft.lifetime -= dt;
    }
    m_floatingTexts.erase(
        std::remove_if(m_floatingTexts.begin(), m_floatingTexts.end(),
                       [](const FloatingText& ft) { return ft.lifetime <= 0.0f; }),
        m_floatingTexts.end()
    );
}

void Game::render(float alpha) {
    if (m_screenShake > 0.1f) {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> offsetDist(-m_screenShake, m_screenShake);
        sf::View shakenView = m_defaultView;
        shakenView.move(offsetDist(rng), offsetDist(rng));
        m_window.setView(shakenView);
    } else {
        m_window.setView(m_defaultView);
    }

    m_window.clear(sf::Color(14, 17, 24));

    // 1. Render arena grid
    m_ui.renderGrid(m_window, PLAY_MIN_X, PLAY_MAX_X, PLAY_MIN_Y, PLAY_MAX_Y, CELL_SIZE);

    // 2. Render Food
    m_food.render(m_window, CELL_SIZE);

    // 3. Render Obstacles
    m_obstacles.render(m_window, CELL_SIZE);

    // 4. Render Snake
    m_snake.render(m_window, alpha, CELL_SIZE);

    // 5. Render Particle Bursts
    for (const auto& p : m_particles) {
        float lifeRatio = std::clamp(p.lifetime / p.maxLifetime, 0.0f, 1.0f);
        sf::CircleShape partShape(p.size * lifeRatio);
        partShape.setOrigin(p.size * lifeRatio, p.size * lifeRatio);
        partShape.setPosition(p.position);

        sf::Color c = p.color;
        c.a = static_cast<sf::Uint8>(lifeRatio * 255.0f);
        partShape.setFillColor(c);
        m_window.draw(partShape);
    }

    // 6. Render Floating Texts
    for (const auto& ft : m_floatingTexts) {
        float lifeRatio = std::clamp(ft.lifetime / ft.maxLifetime, 0.0f, 1.0f);
        m_ui.renderFloatingText(m_window, ft.text, ft.position, ft.color, lifeRatio);
    }

    m_window.setView(m_defaultView);

    // 7. Render top HUD
    m_ui.renderHUD(m_window, static_cast<int>(std::round(m_displayedScore)),
                   m_highScore, m_level, m_gameTime, WINDOW_WIDTH, HUD_HEIGHT);

    // 8. Render Overlays
    switch (m_state) {
        case GameState::Start:
            m_ui.renderStartScreen(m_window, WINDOW_WIDTH, WINDOW_HEIGHT, m_startBlinkTimer);
            break;
        case GameState::Paused:
            m_ui.renderPauseScreen(m_window, WINDOW_WIDTH, WINDOW_HEIGHT);
            break;
        case GameState::GameOver:
            m_ui.renderGameOverScreen(m_window, m_score, m_highScore, m_gameTime, WINDOW_WIDTH, WINDOW_HEIGHT);
            break;
        case GameState::Win:
            m_ui.renderWinScreen(m_window, m_score, m_gameTime, WINDOW_WIDTH, WINDOW_HEIGHT);
            break;
        case GameState::Playing:
        case GameState::Dying:
            break;
    }

    m_window.display();
}

void Game::triggerGameOver() {
    m_state = GameState::GameOver;
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
