#include "Food.hpp"
#include <vector>
#include <cmath>

Food::Food()
    : m_position(-1, -1),
      m_valid(false),
      m_pulseTimer(0.0f),
      m_rng(std::random_device{}()) {
}

bool Food::respawn(const std::deque<sf::Vector2i>& snakeBody,
                   int minX, int maxX, int minY, int maxY) {
    std::vector<sf::Vector2i> availableCells;
    availableCells.reserve((maxX - minX + 1) * (maxY - minY + 1));

    // Collect all grid cells that are currently unoccupied
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            sf::Vector2i candidate(x, y);

            bool occupied = false;
            for (const auto& segment : snakeBody) {
                if (segment == candidate) {
                    occupied = true;
                    break;
                }
            }

            if (!occupied) {
                availableCells.push_back(candidate);
            }
        }
    }

    // If no cells are available, the player has won the game!
    if (availableCells.empty()) {
        m_valid = false;
        return false;
    }

    std::uniform_int_distribution<size_t> dist(0, availableCells.size() - 1);
    m_position = availableCells[dist(m_rng)];
    m_valid = true;
    return true;
}

void Food::updateAnimation(float dt) {
    m_pulseTimer += dt;
    if (m_pulseTimer > 6.2831853f) { // Wrap around 2*pi
        m_pulseTimer -= 6.2831853f;
    }
}

void Food::render(sf::RenderWindow& window, int cellSize) const {
    if (!m_valid) return;

    float centerX = (m_position.x + 0.5f) * static_cast<float>(cellSize);
    float centerY = (m_position.y + 0.5f) * static_cast<float>(cellSize);

    // Subtle sinusoidal breathing animation
    float pulse = 1.0f + 0.10f * std::sin(m_pulseTimer * 6.0f);
    float radius = (static_cast<float>(cellSize) * 0.40f) * pulse;

    // Glowing outer halo
    sf::CircleShape glow(radius + 2.0f);
    glow.setOrigin(radius + 2.0f, radius + 2.0f);
    glow.setPosition(centerX, centerY);
    glow.setFillColor(sf::Color(255, 75, 95, 60));
    window.draw(glow);

    // Main food apple body
    sf::CircleShape foodShape(radius);
    foodShape.setOrigin(radius, radius);
    foodShape.setPosition(centerX, centerY);
    foodShape.setFillColor(sf::Color(235, 60, 75)); // Ruby Red
    foodShape.setOutlineThickness(1.0f);
    foodShape.setOutlineColor(sf::Color(255, 120, 130));
    window.draw(foodShape);

    // Cute green leaf
    sf::RectangleShape leaf(sf::Vector2f(radius * 0.55f, radius * 0.35f));
    leaf.setOrigin(0.0f, radius * 0.35f * 0.5f);
    leaf.setPosition(centerX, centerY - radius * 0.85f);
    leaf.setRotation(-35.0f);
    leaf.setFillColor(sf::Color(46, 204, 113));
    window.draw(leaf);

    // Top-left shine highlight
    sf::CircleShape shine(radius * 0.28f);
    shine.setOrigin(radius * 0.28f, radius * 0.28f);
    shine.setPosition(centerX - radius * 0.35f, centerY - radius * 0.35f);
    shine.setFillColor(sf::Color(255, 255, 255, 190));
    window.draw(shine);
}

const sf::Vector2i& Food::getPosition() const {
    return m_position;
}

bool Food::isValid() const {
    return m_valid;
}

