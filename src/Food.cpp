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
                   const std::vector<sf::Vector2i>& obstacles,
                   int minX, int maxX, int minY, int maxY) {
    std::vector<sf::Vector2i> availableCells;
    availableCells.reserve((maxX - minX + 1) * (maxY - minY + 1));

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
                for (const auto& obs : obstacles) {
                    if (obs == candidate) {
                        occupied = true;
                        break;
                    }
                }
            }

            if (!occupied) {
                availableCells.push_back(candidate);
            }
        }
    }

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
    if (m_pulseTimer > 6.2831853f) {
        m_pulseTimer -= 6.2831853f;
    }
}

sf::Vector2f Food::getPixelPosition(int cellSize) const {
    return sf::Vector2f(
        (static_cast<float>(m_position.x) + 0.5f) * static_cast<float>(cellSize),
        (static_cast<float>(m_position.y) + 0.5f) * static_cast<float>(cellSize)
    );
}

void Food::render(sf::RenderWindow& window, int cellSize) const {
    if (!m_valid) return;

    float baseX = (static_cast<float>(m_position.x) + 0.5f) * static_cast<float>(cellSize);
    float baseY = (static_cast<float>(m_position.y) + 0.5f) * static_cast<float>(cellSize);

    // Subtle floating bobbing
    float bob = std::sin(m_pulseTimer * 4.5f) * 1.5f;
    float centerX = baseX;
    float centerY = baseY + bob;

    // Breathing pulse scale
    float pulse = 1.0f + 0.08f * std::sin(m_pulseTimer * 6.0f);
    float radius = (static_cast<float>(cellSize) * 0.40f) * pulse;

    // 1. Soft Drop Shadow
    sf::CircleShape shadow(radius * 0.9f);
    shadow.setOrigin(radius * 0.9f, radius * 0.9f);
    shadow.setPosition(baseX + 1.5f, baseY + 4.0f);
    shadow.setFillColor(sf::Color(10, 12, 18, 90));
    window.draw(shadow);

    // 2. Multi-layered Glowing Aura
    sf::CircleShape outerGlow(radius + 4.5f);
    outerGlow.setOrigin(radius + 4.5f, radius + 4.5f);
    outerGlow.setPosition(centerX, centerY);
    outerGlow.setFillColor(sf::Color(255, 60, 80, 45));
    window.draw(outerGlow);

    sf::CircleShape innerGlow(radius + 2.0f);
    innerGlow.setOrigin(radius + 2.0f, radius + 2.0f);
    innerGlow.setPosition(centerX, centerY);
    innerGlow.setFillColor(sf::Color(255, 80, 100, 75));
    window.draw(innerGlow);

    // 3. Apple Body
    sf::CircleShape foodShape(radius);
    foodShape.setOrigin(radius, radius);
    foodShape.setPosition(centerX, centerY);
    foodShape.setFillColor(sf::Color(235, 55, 75)); // Ruby Red
    foodShape.setOutlineThickness(1.2f);
    foodShape.setOutlineColor(sf::Color(255, 110, 130));
    window.draw(foodShape);

    // 4. Little brown stem
    sf::RectangleShape stem(sf::Vector2f(2.0f, radius * 0.65f));
    stem.setOrigin(1.0f, radius * 0.65f);
    stem.setPosition(centerX, centerY - radius * 0.85f);
    stem.setRotation(-10.0f);
    stem.setFillColor(sf::Color(140, 85, 45));
    window.draw(stem);

    // 5. Leaf
    sf::RectangleShape leaf(sf::Vector2f(radius * 0.55f, radius * 0.35f));
    leaf.setOrigin(0.0f, radius * 0.35f * 0.5f);
    leaf.setPosition(centerX + 1.0f, centerY - radius * 0.95f);
    leaf.setRotation(-35.0f);
    leaf.setFillColor(sf::Color(46, 204, 113));
    leaf.setOutlineThickness(0.6f);
    leaf.setOutlineColor(sf::Color(35, 160, 85));
    window.draw(leaf);

    // 6. Shiny Glint
    sf::CircleShape shine(radius * 0.28f);
    shine.setOrigin(radius * 0.28f, radius * 0.28f);
    shine.setPosition(centerX - radius * 0.38f, centerY - radius * 0.38f);
    shine.setFillColor(sf::Color(255, 255, 255, 210));
    window.draw(shine);
}

const sf::Vector2i& Food::getPosition() const {
    return m_position;
}

bool Food::isValid() const {
    return m_valid;
}
