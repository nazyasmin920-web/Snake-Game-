#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <random>

class Food {
public:
    Food();

    // Spawns food at a random empty cell. Returns false if no empty cells remain (Win condition)
    bool respawn(const std::deque<sf::Vector2i>& snakeBody,
                 int minX, int maxX, int minY, int maxY);

    // Update pulsing animation
    void updateAnimation(float dt);

    // Render food with subtle pulse effect and arcade styling
    void render(sf::RenderWindow& window, int cellSize) const;

    const sf::Vector2i& getPosition() const;
    bool isValid() const;

private:
    sf::Vector2i m_position;
    bool m_valid;
    float m_pulseTimer;
    std::mt19937 m_rng;
};

