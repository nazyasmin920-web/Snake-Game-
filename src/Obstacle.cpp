#include "Obstacle.hpp"
#include <algorithm>
#include <cmath>

ObstacleManager::ObstacleManager()
    : m_pulseTimer(0.0f),
      m_rng(std::random_device{}()) {
}

void ObstacleManager::clear() {
    m_obstacles.clear();
}

bool ObstacleManager::collidesWith(const sf::Vector2i& cell) const {
    for (const auto& obs : m_obstacles) {
        if (obs == cell) {
            return true;
        }
    }
    return false;
}

const std::vector<sf::Vector2i>& ObstacleManager::getObstacles() const {
    return m_obstacles;
}

size_t ObstacleManager::getCount() const {
    return m_obstacles.size();
}

void ObstacleManager::updateAnimation(float dt) {
    m_pulseTimer += dt;
    if (m_pulseTimer > 6.2831853f) {
        m_pulseTimer -= 6.2831853f;
    }
}

void ObstacleManager::updateForLevel(int level, const sf::Vector2i& foodPos,
                                   const std::deque<sf::Vector2i>& snakeBody,
                                   int minX, int maxX, int minY, int maxY) {
    if (level < 6) {
        m_obstacles.clear();
        return;
    }

    // Always keep exactly 1 obstacle at a time
    size_t targetCount = 1;

    // Ensure we maintain at least targetCount obstacles
    while (m_obstacles.size() < targetCount) {
        onFoodSpawned(foodPos, snakeBody, minX, maxX, minY, maxY, level);
    }
}

void ObstacleManager::onFoodSpawned(const sf::Vector2i& foodPos,
                                   const std::deque<sf::Vector2i>& snakeBody,
                                   int minX, int maxX, int minY, int maxY,
                                   int level) {
    if (level < 6) return;

    size_t maxObstacles = 1; // Always exactly 1 obstacle at a time

    // Offsets 2 to 4 cells away from the food
    const std::vector<sf::Vector2i> offsets = {
        sf::Vector2i(-2, 0), sf::Vector2i(2, 0),
        sf::Vector2i(0, -2), sf::Vector2i(0, 2),
        sf::Vector2i(-2, -2), sf::Vector2i(2, -2),
        sf::Vector2i(-2, 2), sf::Vector2i(2, 2),
        sf::Vector2i(-3, 0), sf::Vector2i(3, 0),
        sf::Vector2i(0, -3), sf::Vector2i(0, 3)
    };

    std::vector<sf::Vector2i> validCandidates;

    sf::Vector2i snakeHead = snakeBody.empty() ? sf::Vector2i(-99, -99) : snakeBody.front();

    for (const auto& offset : offsets) {
        sf::Vector2i candidate = foodPos + offset;

        // Check arena bounds
        if (candidate.x < minX || candidate.x > maxX ||
            candidate.y < minY || candidate.y > maxY) {
            continue;
        }

        // Cannot be on food
        if (candidate == foodPos) continue;

        // Cannot be on or immediately next to the snake head (safe spawn)
        if (std::abs(candidate.x - snakeHead.x) <= 2 &&
            std::abs(candidate.y - snakeHead.y) <= 2) {
            continue;
        }

        // Cannot be on snake body
        bool onSnake = false;
        for (const auto& segment : snakeBody) {
            if (segment == candidate) {
                onSnake = true;
                break;
            }
        }
        if (onSnake) continue;

        // Cannot already be an obstacle
        if (collidesWith(candidate)) continue;

        validCandidates.push_back(candidate);
    }

    if (!validCandidates.empty()) {
        std::uniform_int_distribution<size_t> dist(0, validCandidates.size() - 1);
        sf::Vector2i chosen = validCandidates[dist(m_rng)];

        if (m_obstacles.size() >= maxObstacles) {
            // Replace the oldest obstacle so obstacles dynamically migrate near new food
            m_obstacles.erase(m_obstacles.begin());
        }
        m_obstacles.push_back(chosen);
    }
}

void ObstacleManager::render(sf::RenderWindow& window, int cellSize) const {
    if (m_obstacles.empty()) return;

    const float pad = 1.5f;
    const float size = static_cast<float>(cellSize) - (pad * 2.0f);

    float pulse = 0.5f + 0.5f * std::sin(m_pulseTimer * 5.0f);

    for (const auto& obs : m_obstacles) {
        float px = static_cast<float>(obs.x * cellSize) + pad;
        float py = static_cast<float>(obs.y * cellSize) + pad;
        float centerX = px + size / 2.0f;
        float centerY = py + size / 2.0f;

        // 1. Soft Drop Shadow
        sf::RectangleShape shadow(sf::Vector2f(size, size));
        shadow.setPosition(px + 2.0f, py + 3.0f);
        shadow.setFillColor(sf::Color(8, 10, 16, 120));
        window.draw(shadow);

        // 2. Glowing Hazard Aura
        sf::RectangleShape aura(sf::Vector2f(size + 4.0f, size + 4.0f));
        aura.setOrigin(2.0f, 2.0f);
        aura.setPosition(px, py);
        aura.setFillColor(sf::Color::Transparent);
        aura.setOutlineThickness(1.5f);
        aura.setOutlineColor(sf::Color(255, 90, 20, static_cast<sf::Uint8>(60 + 50 * pulse)));
        window.draw(aura);

        // 3. Base Obsidian Block
        sf::RectangleShape block(sf::Vector2f(size, size));
        block.setPosition(px, py);
        block.setFillColor(sf::Color(35, 38, 48)); // Dark industrial slate
        block.setOutlineThickness(1.2f);
        block.setOutlineColor(sf::Color(255, 140, 0)); // Hazard Amber
        window.draw(block);

        // 4. Diagonal Hazard Stripes
        sf::RectangleShape stripe(sf::Vector2f(size * 1.1f, 3.0f));
        stripe.setOrigin(0.0f, 1.5f);
        stripe.setPosition(px, py + size * 0.35f);
        stripe.setRotation(45.0f);
        stripe.setFillColor(sf::Color(241, 196, 15, 190));
        window.draw(stripe);

        sf::RectangleShape stripe2(sf::Vector2f(size * 1.1f, 3.0f));
        stripe2.setOrigin(0.0f, 1.5f);
        stripe2.setPosition(px + size * 0.35f, py);
        stripe2.setRotation(45.0f);
        stripe2.setFillColor(sf::Color(241, 196, 15, 190));
        window.draw(stripe2);

        // 5. Center Warning Diamond / Icon
        sf::CircleShape diamond(3.5f, 4); // 4 points = diamond
        diamond.setOrigin(3.5f, 3.5f);
        diamond.setPosition(centerX, centerY);
        diamond.setRotation(45.0f);
        diamond.setFillColor(sf::Color(255, 45, 75)); // Red warning center
        window.draw(diamond);
    }
}

