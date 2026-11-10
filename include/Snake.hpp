#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <queue>

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

    // Queue a player movement command (safely handles fast turns)
    void queueDirection(Direction newDir);

    // Advance snake logic by one grid step
    void step(bool willGrow);

    // Returns the grid cell where the head will move next
    sf::Vector2i getNextHeadPosition() const;

    // Checks if the next move will cause self-collision
    // willGrow indicates if the tail segment stays on the board
    bool checkSelfCollision(const sf::Vector2i& nextHead, bool willGrow) const;

    // Checks if a given cell collides with the snake's current body
    bool occupiesCell(const sf::Vector2i& cell) const;

    // Accessors
    const sf::Vector2i& getHead() const;
    const std::deque<sf::Vector2i>& getBody() const;
    Direction getDirection() const;
    int getLength() const;

    // Render snake with smooth interpolation between previous and current grid ticks
    void render(sf::RenderWindow& window, float alpha, int cellSize) const;

private:
    // Helper to check if two directions are directly opposing
    static bool isOpposite(Direction a, Direction b);

    // Current and previous body positions for visual interpolation
    std::deque<sf::Vector2i> m_body;
    std::deque<sf::Vector2i> m_prevBody;

    Direction m_direction;
    std::queue<Direction> m_inputQueue;
};

