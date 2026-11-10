#include "Snake.hpp"
#include <algorithm>
#include <cmath>

Snake::Snake()
    : m_direction(Direction::Right) {
    reset(sf::Vector2i(10, 15));
}

void Snake::reset(const sf::Vector2i& startHeadPos) {
    m_body.clear();
    m_prevBody.clear();

    while (!m_inputQueue.empty()) {
        m_inputQueue.pop();
    }

    m_direction = Direction::Right;

    // Start with 3 segments
    m_body.push_back(startHeadPos);
    m_body.push_back(sf::Vector2i(startHeadPos.x - 1, startHeadPos.y));
    m_body.push_back(sf::Vector2i(startHeadPos.x - 2, startHeadPos.y));

    m_prevBody = m_body;
}

void Snake::queueDirection(Direction newDir) {
    // Determine the direction to compare against (either last queued or current)
    Direction lastDir = m_inputQueue.empty() ? m_direction : m_inputQueue.back();

    // Prevent 180-degree turns and duplicate sequential inputs
    if (newDir != lastDir && !isOpposite(newDir, lastDir)) {
        // Limit queue size to 2 commands to prevent stale inputs
        if (m_inputQueue.size() < 2) {
            m_inputQueue.push(newDir);
        }
    }
}

void Snake::step(bool willGrow) {
    // Save current positions for smooth interpolation during rendering
    m_prevBody = m_body;

    // Dequeue next direction if available
    if (!m_inputQueue.empty()) {
        m_direction = m_inputQueue.front();
        m_inputQueue.pop();
    }

    sf::Vector2i nextHead = getNextHeadPosition();

    // Advance head
    m_body.push_front(nextHead);

    // If not growing, remove the tail segment
    if (!willGrow) {
        m_body.pop_back();
    }
}

sf::Vector2i Snake::getNextHeadPosition() const {
    // If there is a queued direction, the next step will use it
    Direction effectiveDir = m_inputQueue.empty() ? m_direction : m_inputQueue.front();
    sf::Vector2i head = m_body.front();

    switch (effectiveDir) {
        case Direction::Up:    return sf::Vector2i(head.x, head.y - 1);
        case Direction::Down:  return sf::Vector2i(head.x, head.y + 1);
        case Direction::Left:  return sf::Vector2i(head.x - 1, head.y);
        case Direction::Right: return sf::Vector2i(head.x + 1, head.y);
    }
    return head;
}

bool Snake::checkSelfCollision(const sf::Vector2i& nextHead, bool willGrow) const {
    if (m_body.empty()) return false;

    // If growing, the tail segment remains on this step, so check all segments.
    // If not growing, the tail vacates its cell, so moving into the tail cell is valid!
    size_t limit = willGrow ? m_body.size() : (m_body.size() > 1 ? m_body.size() - 1 : 1);

    for (size_t i = 0; i < limit; ++i) {
        if (m_body[i] == nextHead) {
            return true;
        }
    }
    return false;
}

bool Snake::occupiesCell(const sf::Vector2i& cell) const {
    for (const auto& segment : m_body) {
        if (segment == cell) {
            return true;
        }
    }
    return false;
}

const sf::Vector2i& Snake::getHead() const {
    return m_body.front();
}

const std::deque<sf::Vector2i>& Snake::getBody() const {
    return m_body;
}

Direction Snake::getDirection() const {
    return m_direction;
}

int Snake::getLength() const {
    return static_cast<int>(m_body.size());
}

bool Snake::isOpposite(Direction a, Direction b) {
    return (a == Direction::Up    && b == Direction::Down)  ||
           (a == Direction::Down  && b == Direction::Up)    ||
           (a == Direction::Left  && b == Direction::Right) ||
           (a == Direction::Right && b == Direction::Left);
}

void Snake::render(sf::RenderWindow& window, float alpha, int cellSize) const {
    alpha = std::clamp(alpha, 0.0f, 1.0f);
    const float pad = 1.0f;
    const float segSize = static_cast<float>(cellSize) - (pad * 2.0f);

    size_t count = m_body.size();
    if (count == 0) return;

    // Draw body segments (from tail to neck)
    for (size_t i = count; i-- > 1;) {
        sf::Vector2f curr(static_cast<float>(m_body[i].x * cellSize) + pad,
                          static_cast<float>(m_body[i].y * cellSize) + pad);

        sf::Vector2f renderPos = curr;
        if (i < m_prevBody.size()) {
            sf::Vector2f prev(static_cast<float>(m_prevBody[i].x * cellSize) + pad,
                              static_cast<float>(m_prevBody[i].y * cellSize) + pad);
            renderPos = prev + (curr - prev) * alpha;
        }

        sf::RectangleShape segRect(sf::Vector2f(segSize, segSize));
        segRect.setPosition(renderPos);

        // Gradient color from vibrant green near neck to darker green at tail
        float t = static_cast<float>(i) / static_cast<float>(count);
        sf::Uint8 r = static_cast<sf::Uint8>(35 + (20 * (1.0f - t)));
        sf::Uint8 g = static_cast<sf::Uint8>(160 + (45 * (1.0f - t)));
        sf::Uint8 b = static_cast<sf::Uint8>(75 + (20 * (1.0f - t)));
        segRect.setFillColor(sf::Color(r, g, b));
        segRect.setOutlineThickness(1.0f);
        segRect.setOutlineColor(sf::Color(25, 120, 55));

        window.draw(segRect);
    }

    // Draw Head
    {
        sf::Vector2f curr(static_cast<float>(m_body[0].x * cellSize) + pad,
                          static_cast<float>(m_body[0].y * cellSize) + pad);

        sf::Vector2f renderPos = curr;
        if (!m_prevBody.empty()) {
            sf::Vector2f prev(static_cast<float>(m_prevBody[0].x * cellSize) + pad,
                              static_cast<float>(m_prevBody[0].y * cellSize) + pad);
            renderPos = prev + (curr - prev) * alpha;
        }

        sf::RectangleShape headRect(sf::Vector2f(segSize, segSize));
        headRect.setPosition(renderPos);
        headRect.setFillColor(sf::Color(46, 204, 113)); // Bright Emerald Green
        headRect.setOutlineThickness(1.0f);
        headRect.setOutlineColor(sf::Color(39, 174, 96));
        window.draw(headRect);

        // Draw animated eyes on the head
        sf::CircleShape eyeWhite(2.5f);
        eyeWhite.setFillColor(sf::Color::White);

        sf::CircleShape pupil(1.2f);
        pupil.setFillColor(sf::Color(20, 20, 30));

        sf::Vector2f eye1Pos, eye2Pos, pupilOffset;

        switch (m_direction) {
            case Direction::Right:
                eye1Pos = renderPos + sf::Vector2f(segSize - 5.0f, 3.0f);
                eye2Pos = renderPos + sf::Vector2f(segSize - 5.0f, segSize - 8.0f);
                pupilOffset = sf::Vector2f(1.0f, 0.5f);
                break;
            case Direction::Left:
                eye1Pos = renderPos + sf::Vector2f(2.0f, 3.0f);
                eye2Pos = renderPos + sf::Vector2f(2.0f, segSize - 8.0f);
                pupilOffset = sf::Vector2f(0.0f, 0.5f);
                break;
            case Direction::Up:
                eye1Pos = renderPos + sf::Vector2f(3.0f, 2.0f);
                eye2Pos = renderPos + sf::Vector2f(segSize - 8.0f, 2.0f);
                pupilOffset = sf::Vector2f(0.5f, 0.0f);
                break;
            case Direction::Down:
                eye1Pos = renderPos + sf::Vector2f(3.0f, segSize - 5.0f);
                eye2Pos = renderPos + sf::Vector2f(segSize - 8.0f, segSize - 5.0f);
                pupilOffset = sf::Vector2f(0.5f, 1.0f);
                break;
        }

        eyeWhite.setPosition(eye1Pos);
        window.draw(eyeWhite);
        pupil.setPosition(eye1Pos + pupilOffset);
        window.draw(pupil);

        eyeWhite.setPosition(eye2Pos);
        window.draw(eyeWhite);
        pupil.setPosition(eye2Pos + pupilOffset);
        window.draw(pupil);
    }
}

