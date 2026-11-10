#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>
#include <random>

class ObstacleManager {
public:
    ObstacleManager();

    // Clears all obstacles
    void clear();

    // Spawns obstacles when reaching/completing every 6 levels
    void updateForLevel(int level, const sf::Vector2i& foodPos,
                       const std::deque<sf::Vector2i>& snakeBody,
                       int minX, int maxX, int minY, int maxY);

    // When food respawns in level >= 6, dynamically adds/positions obstacles near the food
    void onFoodSpawned(const sf::Vector2i& foodPos,
                       const std::deque<sf::Vector2i>& snakeBody,
                       int minX, int maxX, int minY, int maxY,
                       int level);

    // Checks if a given cell collides with any obstacle
    bool collidesWith(const sf::Vector2i& cell) const;

    const std::vector<sf::Vector2i>& getObstacles() const;
    size_t getCount() const;

    void updateAnimation(float dt);

    // Renders the obstacle blocks with 3D drop-shadows, hazard stripes, and warning auras
    void render(sf::RenderWindow& window, int cellSize) const;

private:
    std::vector<sf::Vector2i> m_obstacles;
    float m_pulseTimer;
    std::mt19937 m_rng;
};

