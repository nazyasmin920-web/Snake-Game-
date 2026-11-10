#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <queue>
#include <vector>

// Movement directions for the snake
enum class Direction {
    Up,
    Down,
    Left,
    Right
};

class Snake {
public:
    Snake();

    // Reset snake to starting state
    void reset(const sf::Vector2i& startHeadPos);

    // Queue a player movement command
    void queueDirection(Direction newDir);

    // Advance snake logic by one grid step with screen wrapping
    void step(int minX, int maxX, int minY, int maxY, bool willGrow);

    // Update real-time visual animations
    void updateAnimation(float dt);

    // Level management for dynamic color transitions
    void setLevel(int level);
    int getLevel() const;

    // Mark snake as dead or alive
    void setDead(bool dead);
    bool isDead() const;

    // Trigger digestion bulge that travels down the body
    void triggerDigestBulge();

    // Returns the grid cell where the head will move next (with boundary wrap applied)
    sf::Vector2i getNextHeadPosition(int minX, int maxX, int minY, int maxY) const;

    // Checks if the next move will cause self-collision
    bool checkSelfCollision(const sf::Vector2i& nextHead, bool willGrow) const;

    // Checks if a given cell collides with the snake's current body
    bool occupiesCell(const sf::Vector2i& cell) const;

    // Accessors
    const sf::Vector2i& getHead() const;
    const std::deque<sf::Vector2i>& getBody() const;
    Direction getDirection() const;
    int getLength() const;

    // Render snake with level-based color schemes, smoothstep interpolation, and eyes
    void render(sf::RenderWindow& window, float alpha, int cellSize) const;

private:
    static bool isOpposite(Direction a, Direction b);

    // Grid coordinates
    std::deque<sf::Vector2i> m_body;
    std::deque<sf::Vector2i> m_prevBody;

    Direction m_direction;
    std::queue<Direction> m_inputQueue;

    // Visual animation states
    bool m_isDead;
    float m_deathTimer;
    float m_tongueTimer;
    float m_animationTime;
    int m_level;

    // Digestion bulge positions
    std::vector<int> m_digestBulges;
};
