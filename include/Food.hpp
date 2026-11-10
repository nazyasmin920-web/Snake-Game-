#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>
#include <random>

class Food {
public:
    Food();

    // Spawns food at a random empty cell, avoiding snake and obstacles
    bool respawn(const std::deque<sf::Vector2i>& snakeBody,
                 const std::vector<sf::Vector2i>& obstacles,
                 int minX, int maxX, int minY, int maxY);

    // Update pulsing animation and bobbing
    void updateAnimation(float dt);

    // Render food with dynamic glow, floating bob, drop shadow, stem, and leaf
    void render(sf::RenderWindow& window, int cellSize) const;

    const sf::Vector2i& getPosition() const;
    sf::Vector2f getPixelPosition(int cellSize) const;
    bool isValid() const;

private:
    sf::Vector2i m_position;
    bool m_valid;
    float m_pulseTimer;
    std::mt19937 m_rng;
};
