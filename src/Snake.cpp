#include "Snake.hpp"
#include <algorithm>
#include <cmath>

namespace {
struct SnakePalette {
    sf::Color head;
    sf::Color headOutline;
    sf::Color bodyStart;
    sf::Color bodyEnd;
    sf::Color bodyOutline;
    sf::Color glow;
};

// Converts HSL to sf::Color for dynamic rainbow chroma
sf::Color hslToRgb(float h, float s, float l) {
    float c = (1.0f - std::abs(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;
    float r = 0, g = 0, b = 0;

    if (h < 60)       { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else              { r = c; g = 0; b = x; }

    return sf::Color(
        static_cast<sf::Uint8>((r + m) * 255),
        static_cast<sf::Uint8>((g + m) * 255),
        static_cast<sf::Uint8>((b + m) * 255)
    );
}

SnakePalette getPaletteForLevel(int level, float animTime) {
    SnakePalette p;
    switch (level) {
        case 1: // Emerald Green (Starter)
            p.head        = sf::Color(46, 215, 115);
            p.headOutline = sf::Color(32, 160, 80);
            p.bodyStart   = sf::Color(38, 195, 85);
            p.bodyEnd     = sf::Color(25, 140, 60);
            p.bodyOutline = sf::Color(20, 110, 48);
            p.glow        = sf::Color(46, 224, 120, 75);
            break;

        case 2: // Cyber Cyan / Electric Aqua
            p.head        = sf::Color(0, 229, 255);
            p.headOutline = sf::Color(0, 160, 190);
            p.bodyStart   = sf::Color(0, 195, 225);
            p.bodyEnd     = sf::Color(0, 120, 165);
            p.bodyOutline = sf::Color(0, 90, 130);
            p.glow        = sf::Color(0, 229, 255, 80);
            break;

        case 3: // Neon Purple / Amethyst
            p.head        = sf::Color(190, 95, 255);
            p.headOutline = sf::Color(140, 50, 210);
            p.bodyStart   = sf::Color(165, 80, 240);
            p.bodyEnd     = sf::Color(105, 30, 180);
            p.bodyOutline = sf::Color(80, 20, 140);
            p.glow        = sf::Color(190, 95, 255, 80);
            break;

        case 4: // Solar Gold / Amber Blaze
            p.head        = sf::Color(255, 215, 0);
            p.headOutline = sf::Color(210, 160, 0);
            p.bodyStart   = sf::Color(255, 175, 15);
            p.bodyEnd     = sf::Color(220, 100, 0);
            p.bodyOutline = sf::Color(160, 70, 0);
            p.glow        = sf::Color(255, 215, 0, 80);
            break;

        case 5: // Level 5 Transformation: Crimson Inferno / Ruby Dragon!
            p.head        = sf::Color(255, 45, 75);
            p.headOutline = sf::Color(190, 20, 40);
            p.bodyStart   = sf::Color(230, 35, 65);
            p.bodyEnd     = sf::Color(150, 10, 35);
            p.bodyOutline = sf::Color(110, 5, 25);
            p.glow        = sf::Color(255, 45, 75, 90);
            break;

        default: { // Level 6+: Prismatic Cosmic Rainbow Chroma
            float baseHue = std::fmod(animTime * 65.0f, 360.0f);
            float tailHue = std::fmod(baseHue + 60.0f, 360.0f);
            p.head        = hslToRgb(baseHue, 0.95f, 0.58f);
            p.headOutline = hslToRgb(baseHue, 0.90f, 0.40f);
            p.bodyStart   = hslToRgb(std::fmod(baseHue + 20.0f, 360.0f), 0.90f, 0.52f);
            p.bodyEnd     = hslToRgb(tailHue, 0.90f, 0.42f);
            p.bodyOutline = hslToRgb(tailHue, 0.85f, 0.30f);
            p.glow        = sf::Color(p.head.r, p.head.g, p.head.b, 85);
            break;
        }
    }
    return p;
}
} // anonymous namespace

Snake::Snake()
    : m_direction(Direction::Right),
      m_isDead(false),
      m_deathTimer(0.0f),
      m_tongueTimer(0.0f),
      m_animationTime(0.0f),
      m_level(1) {
    reset(sf::Vector2i(10, 15));
}

void Snake::reset(const sf::Vector2i& startHeadPos) {
    m_body.clear();
    m_prevBody.clear();
    m_digestBulges.clear();

    while (!m_inputQueue.empty()) {
        m_inputQueue.pop();
    }

    m_direction = Direction::Right;
    m_isDead = false;
    m_deathTimer = 0.0f;
    m_tongueTimer = 0.0f;
    m_animationTime = 0.0f;
    m_level = 1;

    // Start with 3 segments
    m_body.push_back(startHeadPos);
    m_body.push_back(sf::Vector2i(startHeadPos.x - 1, startHeadPos.y));
    m_body.push_back(sf::Vector2i(startHeadPos.x - 2, startHeadPos.y));

    m_prevBody = m_body;
}

void Snake::setLevel(int level) {
    m_level = level;
}

int Snake::getLevel() const {
    return m_level;
}

void Snake::setDead(bool dead) {
    m_isDead = dead;
    if (dead) {
        m_deathTimer = 0.0f;
    }
}

bool Snake::isDead() const {
    return m_isDead;
}

void Snake::triggerDigestBulge() {
    m_digestBulges.push_back(0);
}

void Snake::updateAnimation(float dt) {
    m_animationTime += dt;
    if (m_isDead) {
        m_deathTimer += dt;
    } else {
        m_tongueTimer += dt;
        if (m_tongueTimer > 2.8f) {
            m_tongueTimer = 0.0f;
        }
    }
}

void Snake::queueDirection(Direction newDir) {
    if (m_isDead) return;

    Direction lastDir = m_inputQueue.empty() ? m_direction : m_inputQueue.back();

    if (newDir != lastDir && !isOpposite(newDir, lastDir)) {
        if (m_inputQueue.size() < 2) {
            m_inputQueue.push(newDir);
        }
    }
}

void Snake::step(int minX, int maxX, int minY, int maxY, bool willGrow) {
    m_prevBody = m_body;

    if (!m_inputQueue.empty()) {
        m_direction = m_inputQueue.front();
        m_inputQueue.pop();
    }

    sf::Vector2i nextHead = getNextHeadPosition(minX, maxX, minY, maxY);
    m_body.push_front(nextHead);

    if (!willGrow) {
        m_body.pop_back();
    }

    for (auto& index : m_digestBulges) {
        index++;
    }
    m_digestBulges.erase(
        std::remove_if(m_digestBulges.begin(), m_digestBulges.end(),
                       [this](int idx) { return idx >= static_cast<int>(m_body.size()); }),
        m_digestBulges.end()
    );
}

sf::Vector2i Snake::getNextHeadPosition(int minX, int maxX, int minY, int maxY) const {
    Direction effectiveDir = m_inputQueue.empty() ? m_direction : m_inputQueue.front();
    sf::Vector2i head = m_body.front();

    switch (effectiveDir) {
        case Direction::Up:    head.y--; break;
        case Direction::Down:  head.y++; break;
        case Direction::Left:  head.x--; break;
        case Direction::Right: head.x++; break;
    }

    // Screen wrapping around boundary lines (Snake does NOT die on the lines)
    if (head.x < minX) head.x = maxX;
    else if (head.x > maxX) head.x = minX;

    if (head.y < minY) head.y = maxY;
    else if (head.y > maxY) head.y = minY;

    return head;
}

bool Snake::checkSelfCollision(const sf::Vector2i& nextHead, bool willGrow) const {
    if (m_body.empty()) return false;

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
    size_t count = m_body.size();
    if (count == 0) return;

    alpha = std::clamp(alpha, 0.0f, 1.0f);
    float smoothAlpha = alpha * alpha * (3.0f - 2.0f * alpha);

    SnakePalette pal = getPaletteForLevel(m_level, m_animationTime);

    // Compute interpolated center positions for all segments with wrap smoothing
    std::vector<sf::Vector2f> positions(count);
    for (size_t i = 0; i < count; ++i) {
        sf::Vector2f curr(
            (static_cast<float>(m_body[i].x) + 0.5f) * static_cast<float>(cellSize),
            (static_cast<float>(m_body[i].y) + 0.5f) * static_cast<float>(cellSize)
        );

        if (i < m_prevBody.size()) {
            sf::Vector2f prev(
                (static_cast<float>(m_prevBody[i].x) + 0.5f) * static_cast<float>(cellSize),
                (static_cast<float>(m_prevBody[i].y) + 0.5f) * static_cast<float>(cellSize)
            );

            int dx = m_body[i].x - m_prevBody[i].x;
            int dy = m_body[i].y - m_prevBody[i].y;

            if (dx > 1) {
                prev.x = curr.x + static_cast<float>(cellSize);
            } else if (dx < -1) {
                prev.x = curr.x - static_cast<float>(cellSize);
            }

            if (dy > 1) {
                prev.y = curr.y + static_cast<float>(cellSize);
            } else if (dy < -1) {
                prev.y = curr.y - static_cast<float>(cellSize);
            }

            positions[i] = prev + (curr - prev) * smoothAlpha;
        } else {
            positions[i] = curr;
        }
    }

    const float baseRadius = static_cast<float>(cellSize) * 0.44f;
    const sf::Vector2f shadowOffset(2.0f, 3.5f);

    // 1. Render Drop Shadows
    for (size_t i = count; i-- > 0;) {
        float r = (i == 0) ? (baseRadius * 1.08f) : (baseRadius * (1.0f - 0.25f * (static_cast<float>(i) / count)));
        sf::CircleShape shadow(r);
        shadow.setOrigin(r, r);
        shadow.setPosition(positions[i] + shadowOffset);
        shadow.setFillColor(sf::Color(10, 12, 18, 90));
        window.draw(shadow);
    }

    // 2. Render smooth connectors between adjacent segments
    for (size_t i = 0; i < count - 1; ++i) {
        sf::Vector2f p1 = positions[i];
        sf::Vector2f p2 = positions[i + 1];
        sf::Vector2f diff = p2 - p1;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (dist > 0.001f && dist < static_cast<float>(cellSize * 2)) {
            float connectorThickness = (baseRadius * 1.7f) * (1.0f - 0.2f * (static_cast<float>(i) / count));
            sf::RectangleShape connector(sf::Vector2f(dist, connectorThickness));
            connector.setOrigin(0.0f, connectorThickness / 2.0f);
            connector.setPosition(p1);
            float angle = std::atan2(diff.y, diff.x) * 180.0f / 3.14159265f;
            connector.setRotation(angle);

            float t = static_cast<float>(i) / static_cast<float>(count);
            sf::Color connColor;
            if (m_isDead) {
                float flash = (std::sin(m_deathTimer * 16.0f) + 1.0f) * 0.5f;
                connColor = sf::Color(static_cast<sf::Uint8>(180 + 75 * flash),
                                      static_cast<sf::Uint8>(50 * (1.0f - flash)),
                                      static_cast<sf::Uint8>(50 * (1.0f - flash)),
                                      200);
            } else {
                sf::Uint8 rCol = static_cast<sf::Uint8>(pal.bodyStart.r + (pal.bodyEnd.r - pal.bodyStart.r) * t);
                sf::Uint8 gCol = static_cast<sf::Uint8>(pal.bodyStart.g + (pal.bodyEnd.g - pal.bodyStart.g) * t);
                sf::Uint8 bCol = static_cast<sf::Uint8>(pal.bodyStart.b + (pal.bodyEnd.b - pal.bodyStart.b) * t);
                connColor = sf::Color(rCol, gCol, bCol);
            }
            connector.setFillColor(connColor);
            window.draw(connector);
        }
    }

    // 3. Render Body Segments
    for (size_t i = count; i-- > 1;) {
        float bulgeScale = 1.0f;
        for (int bulgeIdx : m_digestBulges) {
            if (bulgeIdx == static_cast<int>(i)) {
                bulgeScale = 1.25f;
                break;
            }
        }

        float t = static_cast<float>(i) / static_cast<float>(count);
        float radius = (baseRadius * (1.0f - 0.22f * t)) * bulgeScale;

        sf::CircleShape segment(radius);
        segment.setOrigin(radius, radius);
        segment.setPosition(positions[i]);

        sf::Color segColor;
        if (m_isDead) {
            float flash = (std::sin(m_deathTimer * 16.0f + i * 0.4f) + 1.0f) * 0.5f;
            segColor = sf::Color(static_cast<sf::Uint8>(210 + 45 * flash),
                                 static_cast<sf::Uint8>(50 * (1.0f - flash)),
                                 static_cast<sf::Uint8>(50 * (1.0f - flash)),
                                 static_cast<sf::Uint8>(std::max(40.0f, 255.0f - m_deathTimer * 120.0f)));
        } else {
            sf::Uint8 rCol = static_cast<sf::Uint8>(pal.bodyStart.r + (pal.bodyEnd.r - pal.bodyStart.r) * t);
            sf::Uint8 gCol = static_cast<sf::Uint8>(pal.bodyStart.g + (pal.bodyEnd.g - pal.bodyStart.g) * t);
            sf::Uint8 bCol = static_cast<sf::Uint8>(pal.bodyStart.b + (pal.bodyEnd.b - pal.bodyStart.b) * t);
            segColor = sf::Color(rCol, gCol, bCol);
        }

        segment.setFillColor(segColor);
        segment.setOutlineThickness(1.0f);
        segment.setOutlineColor(m_isDead ? sf::Color(140, 20, 20) : pal.bodyOutline);
        window.draw(segment);

        if (!m_isDead) {
            float glintRadius = radius * 0.32f;
            sf::CircleShape glint(glintRadius);
            glint.setOrigin(glintRadius, glintRadius);
            glint.setPosition(positions[i] - sf::Vector2f(radius * 0.35f, radius * 0.35f));
            glint.setFillColor(sf::Color(255, 255, 255, 125));
            window.draw(glint);
        }
    }

    // 4. Render Head
    {
        sf::Vector2f headPos = positions[0];
        float headRadius = baseRadius * 1.12f;

        // Animated flicking tongue
        if (!m_isDead && m_tongueTimer > 2.3f && m_tongueTimer < 2.7f) {
            float tongueProgress = (m_tongueTimer - 2.3f) / 0.4f;
            float tongueLen = std::sin(tongueProgress * 3.14159f) * 11.0f;

            sf::Vector2f tongueDir;
            switch (m_direction) {
                case Direction::Up:    tongueDir = sf::Vector2f(0.0f, -1.0f); break;
                case Direction::Down:  tongueDir = sf::Vector2f(0.0f, 1.0f);  break;
                case Direction::Left:  tongueDir = sf::Vector2f(-1.0f, 0.0f); break;
                case Direction::Right: tongueDir = sf::Vector2f(1.0f, 0.0f);  break;
            }

            sf::Vector2f tongueStart = headPos + tongueDir * headRadius;
            sf::Vector2f tongueEnd = tongueStart + tongueDir * tongueLen;

            sf::RectangleShape tongueStem(sf::Vector2f(tongueLen, 2.0f));
            tongueStem.setOrigin(0.0f, 1.0f);
            tongueStem.setPosition(tongueStart);
            float angle = std::atan2(tongueDir.y, tongueDir.x) * 180.0f / 3.14159265f;
            tongueStem.setRotation(angle);
            tongueStem.setFillColor(sf::Color(231, 76, 60));
            window.draw(tongueStem);

            sf::RectangleShape fork1(sf::Vector2f(3.5f, 1.5f));
            fork1.setPosition(tongueEnd);
            fork1.setRotation(angle - 30.0f);
            fork1.setFillColor(sf::Color(231, 76, 60));
            window.draw(fork1);

            sf::RectangleShape fork2(sf::Vector2f(3.5f, 1.5f));
            fork2.setPosition(tongueEnd);
            fork2.setRotation(angle + 30.0f);
            fork2.setFillColor(sf::Color(231, 76, 60));
            window.draw(fork2);
        }

        // Head glowing outer aura
        sf::CircleShape glow(headRadius + 2.5f);
        glow.setOrigin(headRadius + 2.5f, headRadius + 2.5f);
        glow.setPosition(headPos);
        glow.setFillColor(m_isDead ? sf::Color(240, 40, 40, 80) : pal.glow);
        window.draw(glow);

        // Head main sphere
        sf::CircleShape headShape(headRadius);
        headShape.setOrigin(headRadius, headRadius);
        headShape.setPosition(headPos);
        headShape.setFillColor(m_isDead ? sf::Color(230, 60, 60) : pal.head);
        headShape.setOutlineThickness(1.5f);
        headShape.setOutlineColor(m_isDead ? sf::Color(180, 20, 20) : pal.headOutline);
        window.draw(headShape);

        if (!m_isDead) {
            sf::CircleShape headHighlight(headRadius * 0.35f);
            headHighlight.setOrigin(headRadius * 0.35f, headRadius * 0.35f);
            headHighlight.setPosition(headPos - sf::Vector2f(headRadius * 0.35f, headRadius * 0.35f));
            headHighlight.setFillColor(sf::Color(255, 255, 255, 140));
            window.draw(headHighlight);
        }

        // Head Eyes
        sf::Vector2f eye1Offset, eye2Offset, pupilDir;
        float eyeSpacing = 4.5f;
        float eyeForward = 3.5f;

        switch (m_direction) {
            case Direction::Right:
                eye1Offset = sf::Vector2f(eyeForward, -eyeSpacing);
                eye2Offset = sf::Vector2f(eyeForward, eyeSpacing);
                pupilDir   = sf::Vector2f(1.4f, 0.0f);
                break;
            case Direction::Left:
                eye1Offset = sf::Vector2f(-eyeForward, -eyeSpacing);
                eye2Offset = sf::Vector2f(-eyeForward, eyeSpacing);
                pupilDir   = sf::Vector2f(-1.4f, 0.0f);
                break;
            case Direction::Up:
                eye1Offset = sf::Vector2f(-eyeSpacing, -eyeForward);
                eye2Offset = sf::Vector2f(eyeSpacing, -eyeForward);
                pupilDir   = sf::Vector2f(0.0f, -1.4f);
                break;
            case Direction::Down:
                eye1Offset = sf::Vector2f(-eyeSpacing, eyeForward);
                eye2Offset = sf::Vector2f(eyeSpacing, eyeForward);
                pupilDir   = sf::Vector2f(0.0f, 1.4f);
                break;
        }

        if (m_isDead) {
            auto drawCrossEye = [&](const sf::Vector2f& center) {
                float s = 3.2f;
                sf::RectangleShape l1(sf::Vector2f(s * 2.0f, 1.8f));
                l1.setOrigin(s, 0.9f);
                l1.setPosition(center);
                l1.setRotation(45.0f);
                l1.setFillColor(sf::Color(30, 30, 40));
                window.draw(l1);

                sf::RectangleShape l2(sf::Vector2f(s * 2.0f, 1.8f));
                l2.setOrigin(s, 0.9f);
                l2.setPosition(center);
                l2.setRotation(-45.0f);
                l2.setFillColor(sf::Color(30, 30, 40));
                window.draw(l2);
            };
            drawCrossEye(headPos + eye1Offset);
            drawCrossEye(headPos + eye2Offset);
        } else {
            float eyeR = 3.2f;
            float pupilR = 1.7f;

            // Eye 1
            sf::CircleShape eye1(eyeR);
            eye1.setOrigin(eyeR, eyeR);
            eye1.setPosition(headPos + eye1Offset);
            eye1.setFillColor(sf::Color::White);
            eye1.setOutlineThickness(0.8f);
            eye1.setOutlineColor(sf::Color(30, 30, 40, 180));
            window.draw(eye1);

            sf::CircleShape pupil1(pupilR);
            pupil1.setOrigin(pupilR, pupilR);
            pupil1.setPosition(headPos + eye1Offset + pupilDir);
            pupil1.setFillColor(sf::Color(20, 24, 32));
            window.draw(pupil1);

            sf::CircleShape glint1(0.7f);
            glint1.setOrigin(0.7f, 0.7f);
            glint1.setPosition(headPos + eye1Offset + pupilDir - sf::Vector2f(0.6f, 0.6f));
            glint1.setFillColor(sf::Color::White);
            window.draw(glint1);

            // Eye 2
            sf::CircleShape eye2(eyeR);
            eye2.setOrigin(eyeR, eyeR);
            eye2.setPosition(headPos + eye2Offset);
            eye2.setFillColor(sf::Color::White);
            eye2.setOutlineThickness(0.8f);
            eye2.setOutlineColor(sf::Color(30, 30, 40, 180));
            window.draw(eye2);

            sf::CircleShape pupil2(pupilR);
            pupil2.setOrigin(pupilR, pupilR);
            pupil2.setPosition(headPos + eye2Offset + pupilDir);
            pupil2.setFillColor(sf::Color(20, 24, 32));
            window.draw(pupil2);

            sf::CircleShape glint2(0.7f);
            glint2.setOrigin(0.7f, 0.7f);
            glint2.setPosition(headPos + eye2Offset + pupilDir - sf::Vector2f(0.6f, 0.6f));
            glint2.setFillColor(sf::Color::White);
            window.draw(glint2);
        }
    }
}
