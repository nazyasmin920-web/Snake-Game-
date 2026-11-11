#include "UI.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>

UI::UI()
    : m_fontLoaded(false) {
    loadFont("assets/fonts/font.ttf");
}

std::string UI::formatTime(float totalSeconds) {
    int total = static_cast<int>(totalSeconds);
    if (total < 0) total = 0;
    int mins = total / 60;
    int secs = total % 60;
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02d:%02d", mins, secs);
    return std::string(buf);
}

bool UI::loadFont(const std::string& preferredPath) {
    const std::vector<std::string> searchPaths = {
        preferredPath,
        "assets/fonts/font.ttf",
        "../assets/fonts/font.ttf",
        "../../assets/fonts/font.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/segoeui.ttf"
    };

    for (const auto& path : searchPaths) {
        if (!path.empty() && m_font.loadFromFile(path)) {
            m_fontLoaded = true;
            return true;
        }
    }

    std::cout << "[UI] Notice: TTF font file could not be opened; using procedural fallback font." << std::endl;
    m_fontLoaded = false;
    return false;
}

void UI::drawCenteredText(sf::RenderWindow& window, const std::string& textStr,
                          float centerY, unsigned int charSize, sf::Color color,
                          bool isBold) const {
    if (m_fontLoaded) {
        sf::Text text;
        text.setFont(m_font);
        text.setString(textStr);
        text.setCharacterSize(charSize);
        text.setFillColor(color);
        if (isBold) {
            text.setStyle(sf::Text::Bold);
        }

        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
        text.setPosition(static_cast<float>(window.getSize().x) / 2.0f, centerY);
        window.draw(text);
    } else {
        float totalWidth = static_cast<float>(textStr.length()) * (charSize * 0.65f);
        float startX = (static_cast<float>(window.getSize().x) - totalWidth) / 2.0f;
        renderFallbackText(window, textStr, startX, centerY - (charSize / 2.0f), static_cast<float>(charSize), color);
    }
}

void UI::renderGrid(sf::RenderWindow& window,
                    int minX, int maxX, int minY, int maxY, int cellSize) const {
    float left = static_cast<float>(minX * cellSize);
    float top = static_cast<float>(minY * cellSize);
    float width = static_cast<float>((maxX - minX + 1) * cellSize);
    float height = static_cast<float>((maxY - minY + 1) * cellSize);

    sf::RectangleShape playArea(sf::Vector2f(width, height));
    playArea.setPosition(left, top);
    playArea.setFillColor(sf::Color(16, 20, 28));
    window.draw(playArea);

    sf::Color gridColor(24, 30, 42);

    for (int x = minX; x <= maxX + 1; ++x) {
        sf::RectangleShape line(sf::Vector2f(1.0f, height));
        line.setPosition(static_cast<float>(x * cellSize), top);
        line.setFillColor(gridColor);
        window.draw(line);
    }

    for (int y = minY; y <= maxY + 1; ++y) {
        sf::RectangleShape line(sf::Vector2f(width, 1.0f));
        line.setPosition(left, static_cast<float>(y * cellSize));
        line.setFillColor(gridColor);
        window.draw(line);
    }

    // Glowing cyan portal wrap boundary
    sf::RectangleShape glowBorder(sf::Vector2f(width + 2.0f, height + 2.0f));
    glowBorder.setPosition(left - 1.0f, top - 1.0f);
    glowBorder.setFillColor(sf::Color::Transparent);
    glowBorder.setOutlineThickness(2.0f);
    glowBorder.setOutlineColor(sf::Color(52, 152, 219, 110));
    window.draw(glowBorder);

    sf::RectangleShape border(sf::Vector2f(width, height));
    border.setPosition(left, top);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(1.5f);
    border.setOutlineColor(sf::Color(41, 128, 185));
    window.draw(border);

    float bracketLen = 14.0f;
    auto drawCorner = [&](float x, float y, float dx, float dy) {
        sf::RectangleShape h(sf::Vector2f(bracketLen * dx, 2.5f));
        h.setPosition(x, y);
        h.setFillColor(sf::Color(52, 152, 219));
        window.draw(h);

        sf::RectangleShape v(sf::Vector2f(2.5f, bracketLen * dy));
        v.setPosition(x, y);
        v.setFillColor(sf::Color(52, 152, 219));
        window.draw(v);
    };

    drawCorner(left, top, 1.0f, 1.0f);
    drawCorner(left + width, top, -1.0f, 1.0f);
    drawCorner(left, top + height, 1.0f, -1.0f);
    drawCorner(left + width, top + height, -1.0f, -1.0f);
}

void UI::renderHUD(sf::RenderWindow& window, int score, int highScore, int level, float gameTime,
                   int windowWidth, int hudHeight) const {
    sf::RectangleShape hudBar(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(hudHeight)));
    hudBar.setPosition(0.0f, 0.0f);
    hudBar.setFillColor(sf::Color(12, 15, 22));
    window.draw(hudBar);

    sf::RectangleShape separator(sf::Vector2f(static_cast<float>(windowWidth), 2.0f));
    separator.setPosition(0.0f, static_cast<float>(hudHeight) - 2.0f);
    separator.setFillColor(sf::Color(52, 152, 219, 190));
    window.draw(separator);

    float textY = static_cast<float>(hudHeight) / 2.0f;

    if (m_fontLoaded) {
        // 1. SCORE CARD
        sf::RectangleShape scoreCard(sf::Vector2f(135.0f, 26.0f));
        scoreCard.setOrigin(0.0f, 13.0f);
        scoreCard.setPosition(12.0f, textY);
        scoreCard.setFillColor(sf::Color(22, 28, 40));
        scoreCard.setOutlineThickness(1.0f);
        scoreCard.setOutlineColor(sf::Color(40, 52, 72));
        window.draw(scoreCard);

        sf::Text scoreText;
        scoreText.setFont(m_font);
        scoreText.setString("SCORE: " + std::to_string(score));
        scoreText.setCharacterSize(14);
        scoreText.setFillColor(sf::Color(236, 240, 241));
        scoreText.setStyle(sf::Text::Bold);
        sf::FloatRect bScore = scoreText.getLocalBounds();
        scoreText.setOrigin(0.0f, bScore.top + bScore.height / 2.0f);
        scoreText.setPosition(22.0f, textY);
        window.draw(scoreText);

        // 2. MATCH TIMER (MM:SS)
        sf::RectangleShape timeCard(sf::Vector2f(135.0f, 26.0f));
        timeCard.setOrigin(0.0f, 13.0f);
        timeCard.setPosition(160.0f, textY);
        timeCard.setFillColor(sf::Color(20, 30, 42));
        timeCard.setOutlineThickness(1.0f);
        timeCard.setOutlineColor(sf::Color(42, 70, 95));
        window.draw(timeCard);

        sf::Text timeText;
        timeText.setFont(m_font);
        timeText.setString("TIME: " + formatTime(gameTime));
        timeText.setCharacterSize(14);
        timeText.setFillColor(sf::Color(0, 229, 255)); // Cyan timer
        timeText.setStyle(sf::Text::Bold);
        sf::FloatRect bTime = timeText.getLocalBounds();
        timeText.setOrigin(0.0f, bTime.top + bTime.height / 2.0f);
        timeText.setPosition(170.0f, textY);
        window.draw(timeText);

        // 3. HIGH SCORE
        sf::RectangleShape highCard(sf::Vector2f(160.0f, 26.0f));
        highCard.setOrigin(80.0f, 13.0f);
        highCard.setPosition(static_cast<float>(windowWidth) / 2.0f + 60.0f, textY);
        highCard.setFillColor(sf::Color(26, 24, 32));
        highCard.setOutlineThickness(1.0f);
        highCard.setOutlineColor(sf::Color(65, 55, 30));
        window.draw(highCard);

        sf::Text highText;
        highText.setFont(m_font);
        highText.setString("HIGH: " + std::to_string(highScore));
        highText.setCharacterSize(14);
        highText.setFillColor(sf::Color(241, 196, 15));
        highText.setStyle(sf::Text::Bold);
        sf::FloatRect bHigh = highText.getLocalBounds();
        highText.setOrigin(bHigh.left + bHigh.width / 2.0f, bHigh.top + bHigh.height / 2.0f);
        highText.setPosition(static_cast<float>(windowWidth) / 2.0f + 60.0f, textY);
        window.draw(highText);

        // 4. LEVEL CARD
        sf::RectangleShape levelCard(sf::Vector2f(145.0f, 26.0f));
        levelCard.setOrigin(145.0f, 13.0f);
        levelCard.setPosition(static_cast<float>(windowWidth) - 12.0f, textY);
        levelCard.setFillColor(sf::Color(18, 28, 24));
        levelCard.setOutlineThickness(1.0f);
        levelCard.setOutlineColor(sf::Color(32, 60, 45));
        window.draw(levelCard);

        // Color badge for current level
        sf::Color lvlColor;
        switch (level) {
            case 1: lvlColor = sf::Color(46, 215, 115); break;
            case 2: lvlColor = sf::Color(0, 229, 255);  break;
            case 3: lvlColor = sf::Color(190, 95, 255); break;
            case 4: lvlColor = sf::Color(255, 215, 0);  break;
            case 5: lvlColor = sf::Color(255, 45, 75);  break;
            default: lvlColor = sf::Color(255, 105, 180); break;
        }

        sf::CircleShape lvlGem(4.5f);
        lvlGem.setOrigin(4.5f, 4.5f);
        lvlGem.setPosition(static_cast<float>(windowWidth) - 140.0f, textY);
        lvlGem.setFillColor(lvlColor);
        window.draw(lvlGem);

        sf::Text levelText;
        levelText.setFont(m_font);
        levelText.setString("LVL " + std::to_string(level));
        levelText.setCharacterSize(14);
        levelText.setFillColor(lvlColor);
        levelText.setStyle(sf::Text::Bold);
        sf::FloatRect bLvl = levelText.getLocalBounds();
        levelText.setOrigin(0.0f, bLvl.top + bLvl.height / 2.0f);
        levelText.setPosition(static_cast<float>(windowWidth) - 130.0f, textY);
        window.draw(levelText);

        // Progress bar inside level badge
        float progress = static_cast<float>(score % 50) / 50.0f;
        sf::RectangleShape barBg(sf::Vector2f(65.0f, 3.5f));
        barBg.setPosition(static_cast<float>(windowWidth) - 82.0f, textY - 1.75f);
        barBg.setFillColor(sf::Color(30, 42, 38));
        window.draw(barBg);

        if (progress > 0.0f) {
            sf::RectangleShape barFill(sf::Vector2f(65.0f * progress, 3.5f));
            barFill.setPosition(static_cast<float>(windowWidth) - 82.0f, textY - 1.75f);
            barFill.setFillColor(lvlColor);
            window.draw(barFill);
        }
    } else {
        renderFallbackText(window, "SCORE: " + std::to_string(score), 20.0f, 10.0f, 15.0f, sf::Color(236, 240, 241));
        renderFallbackText(window, "TIME: " + formatTime(gameTime), 160.0f, 10.0f, 15.0f, sf::Color(0, 229, 255));
        renderFallbackText(window, "HI: " + std::to_string(highScore), 320.0f, 10.0f, 15.0f, sf::Color(241, 196, 15));
        renderFallbackText(window, "LVL: " + std::to_string(level), static_cast<float>(windowWidth) - 100.0f, 10.0f, 15.0f, sf::Color(46, 204, 113));
    }
}

void UI::renderFloatingText(sf::RenderWindow& window, const std::string& text,
                            const sf::Vector2f& pos, sf::Color color, float alpha) const {
    if (!m_fontLoaded) return;

    color.a = static_cast<sf::Uint8>(std::clamp(alpha * 255.0f, 0.0f, 255.0f));

    sf::Text fText;
    fText.setFont(m_font);
    fText.setString(text);
    fText.setCharacterSize(16);
    fText.setFillColor(color);
    fText.setStyle(sf::Text::Bold);

    sf::FloatRect bounds = fText.getLocalBounds();
    fText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    fText.setPosition(pos);
    window.draw(fText);
}

void UI::renderModeSelectScreen(sf::RenderWindow& window, int windowWidth, int windowHeight, float blinkTimer) const {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(10, 13, 22, 240));
    window.draw(overlay);

    drawCenteredText(window, "SNAKE GAME PRO", 80.0f, 44, sf::Color(46, 204, 113), true);
    drawCenteredText(window, "SELECT GAME MODE", 125.0f, 18, sf::Color(52, 152, 219), true);

    float cardW = 340.0f;
    float cardH = 260.0f;
    float cardY = 165.0f;

    // Card 1: CLASSIC MODE
    float card1X = (static_cast<float>(windowWidth) / 2.0f) - cardW - 15.0f;
    sf::RectangleShape card1(sf::Vector2f(cardW, cardH));
    card1.setPosition(card1X, cardY);
    card1.setFillColor(sf::Color(18, 25, 38));
    card1.setOutlineThickness(2.0f);
    card1.setOutlineColor(sf::Color(46, 204, 113));
    window.draw(card1);

    // Card 1 header badge
    sf::RectangleShape badge1(sf::Vector2f(cardW, 40.0f));
    badge1.setPosition(card1X, cardY);
    badge1.setFillColor(sf::Color(28, 48, 40));
    window.draw(badge1);

    // Text for Card 1
    float c1CenterX = card1X + cardW / 2.0f;
    if (m_fontLoaded) {
        sf::Text t;
        t.setFont(m_font);
        t.setCharacterSize(18);
        t.setStyle(sf::Text::Bold);
        t.setFillColor(sf::Color(46, 204, 113));
        t.setString("[ 1 ] CLASSIC MODE");
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
        t.setPosition(c1CenterX, cardY + 20.0f);
        window.draw(t);

        auto drawCardLine = [&](const std::string& line, float y, sf::Color col, unsigned int sz = 13, bool bold = false) {
            sf::Text lt;
            lt.setFont(m_font);
            lt.setCharacterSize(sz);
            if (bold) lt.setStyle(sf::Text::Bold);
            lt.setFillColor(col);
            lt.setString(line);
            sf::FloatRect lb = lt.getLocalBounds();
            lt.setOrigin(lb.left + lb.width / 2.0f, lb.top + lb.height / 2.0f);
            lt.setPosition(c1CenterX, y);
            window.draw(lt);
        };

        drawCardLine("The Original Endless Snake", cardY + 65.0f, sf::Color(241, 196, 15), 14, true);
        drawCardLine("- Infinite gameplay & high score chase", cardY + 100.0f, sf::Color(220, 225, 230));
        drawCardLine("- Level up every 5 food items", cardY + 125.0f, sf::Color(0, 229, 255));
        drawCardLine("- Dynamic colors & Crimson at Lvl 5", cardY + 150.0f, sf::Color(255, 45, 75));
        drawCardLine("- Obstacles appear starting at Level 6", cardY + 175.0f, sf::Color(241, 196, 15));
        drawCardLine("- Pass-through portal wrap borders", cardY + 200.0f, sf::Color(52, 152, 219));
        drawCardLine("PRESS [1] TO CHOOSE", cardY + 235.0f, sf::Color(46, 204, 113), 15, true);
    }

    // Card 2: TIME ATTACK / CHALLENGE MODE
    float card2X = (static_cast<float>(windowWidth) / 2.0f) + 15.0f;
    sf::RectangleShape card2(sf::Vector2f(cardW, cardH));
    card2.setPosition(card2X, cardY);
    card2.setFillColor(sf::Color(25, 20, 35));
    card2.setOutlineThickness(2.0f);
    card2.setOutlineColor(sf::Color(255, 105, 180));
    window.draw(card2);

    // Card 2 header badge
    sf::RectangleShape badge2(sf::Vector2f(cardW, 40.0f));
    badge2.setPosition(card2X, cardY);
    badge2.setFillColor(sf::Color(50, 30, 55));
    window.draw(badge2);

    float c2CenterX = card2X + cardW / 2.0f;
    if (m_fontLoaded) {
        sf::Text t2;
        t2.setFont(m_font);
        t2.setCharacterSize(18);
        t2.setStyle(sf::Text::Bold);
        t2.setFillColor(sf::Color(255, 105, 180));
        t2.setString("[ 2 ] TIME CHALLENGE");
        sf::FloatRect b2 = t2.getLocalBounds();
        t2.setOrigin(b2.left + b2.width / 2.0f, b2.top + b2.height / 2.0f);
        t2.setPosition(c2CenterX, cardY + 20.0f);
        window.draw(t2);

        auto drawCard2Line = [&](const std::string& line, float y, sf::Color col, unsigned int sz = 13, bool bold = false) {
            sf::Text lt;
            lt.setFont(m_font);
            lt.setCharacterSize(sz);
            if (bold) lt.setStyle(sf::Text::Bold);
            lt.setFillColor(col);
            lt.setString(line);
            sf::FloatRect lb = lt.getLocalBounds();
            lt.setOrigin(lb.left + lb.width / 2.0f, lb.top + lb.height / 2.0f);
            lt.setPosition(c2CenterX, y);
            window.draw(lt);
        };

        drawCard2Line("Eat Food Before Time Expires!", cardY + 65.0f, sf::Color(255, 60, 80), 14, true);
        drawCard2Line("- Target quota per wave (5, 7, 9...)", cardY + 100.0f, sf::Color(220, 225, 230));
        drawCard2Line("- Ticking countdown timer!", cardY + 125.0f, sf::Color(255, 75, 95));
        drawCard2Line("- Time bonus rewarded per clear", cardY + 150.0f, sf::Color(241, 196, 15));
        drawCard2Line("- Fast-paced rush against the clock", cardY + 175.0f, sf::Color(0, 229, 255));
        drawCard2Line("- Pure arcade adrenaline challenge", cardY + 200.0f, sf::Color(190, 95, 255));
        drawCard2Line("PRESS [2] TO CHOOSE", cardY + 235.0f, sf::Color(255, 105, 180), 15, true);
    }

    bool showPrompt = std::fmod(blinkTimer, 1.0f) < 0.65f;
    if (showPrompt) {
        drawCenteredText(window, "PRESS [ 1 ] OR [ 2 ] ON KEYBOARD TO SELECT", 475.0f, 18, sf::Color(241, 196, 15), true);
    }
    drawCenteredText(window, "Press ESC to Quit Game", 515.0f, 13, sf::Color(149, 165, 166));
}

void UI::renderStartScreen(sf::RenderWindow& window, int windowWidth, int windowHeight, float blinkTimer) const {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(10, 13, 20, 230));
    window.draw(overlay);

    drawCenteredText(window, "SNAKE", 135.0f, 66, sf::Color(46, 204, 113), true);
    drawCenteredText(window, "PORTAL / PASS-THROUGH EDITION", 190.0f, 17, sf::Color(52, 152, 219), true);

    float boxW = 500.0f;
    float boxH = 205.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = 230.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(18, 24, 36));
    box.setOutlineThickness(1.5f);
    box.setOutlineColor(sf::Color(52, 152, 219));
    window.draw(box);

    drawCenteredText(window, "GAME FEATURES & CONTROLS", boxY + 22.0f, 16, sf::Color(241, 196, 15), true);
    drawCenteredText(window, "W, A, S, D  or  Arrow Keys  :  Move Snake", boxY + 54.0f, 14, sf::Color(220, 225, 230));
    drawCenteredText(window, "Timer  :  Survive as long as possible for higher ranks!", boxY + 84.0f, 14, sf::Color(0, 229, 255));
    drawCenteredText(window, "Level Up  :  Snake changes color every level (Crimson at Lvl 5!)", boxY + 114.0f, 14, sf::Color(255, 45, 75));
    drawCenteredText(window, "Portal Lines  :  Pass freely through boundaries without dying!", boxY + 144.0f, 14, sf::Color(52, 152, 219));
    drawCenteredText(window, "Press M at any time to return to Mode Select", boxY + 174.0f, 14, sf::Color(46, 204, 113));

    bool showPrompt = std::fmod(blinkTimer, 1.0f) < 0.65f;
    if (showPrompt) {
        drawCenteredText(window, "PRESS ENTER OR SPACE TO START", 480.0f, 22, sf::Color(46, 204, 113), true);
    }
}

void UI::renderPauseScreen(sf::RenderWindow& window, int windowWidth, int windowHeight) const {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(8, 10, 16, 195));
    window.draw(overlay);

    float boxW = 340.0f;
    float boxH = 180.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = (static_cast<float>(windowHeight) - boxH) / 2.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(20, 26, 40));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(52, 152, 219));
    window.draw(box);

    drawCenteredText(window, "PAUSED", boxY + 45.0f, 36, sf::Color(241, 196, 15), true);
    drawCenteredText(window, "Press SPACE to Resume", boxY + 105.0f, 17, sf::Color(236, 240, 241));
    drawCenteredText(window, "Press ESC to Quit", boxY + 140.0f, 14, sf::Color(149, 165, 166));
}

void UI::renderGameOverScreen(sf::RenderWindow& window, int score, int highScore, float gameTime,
                              int windowWidth, int windowHeight) const {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(10, 12, 18, 220));
    window.draw(overlay);

    float boxW = 460.0f;
    float boxH = 300.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = (static_cast<float>(windowHeight) - boxH) / 2.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(22, 18, 24));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(231, 76, 60));
    window.draw(box);

    drawCenteredText(window, "GAME OVER", boxY + 40.0f, 36, sf::Color(231, 76, 60), true);

    if (score >= highScore && score > 0) {
        drawCenteredText(window, "* NEW HIGH SCORE! *", boxY + 86.0f, 20, sf::Color(241, 196, 15), true);
    } else {
        drawCenteredText(window, "Final Score: " + std::to_string(score), boxY + 86.0f, 19, sf::Color(236, 240, 241));
    }

    drawCenteredText(window, "Time Survived: " + formatTime(gameTime), boxY + 120.0f, 17, sf::Color(0, 229, 255));
    drawCenteredText(window, "Best Score: " + std::to_string(highScore), boxY + 152.0f, 16, sf::Color(189, 195, 199));
    drawCenteredText(window, "Press ENTER to Restart", boxY + 195.0f, 19, sf::Color(46, 204, 113), true);
    drawCenteredText(window, "Press M for Mode Select", boxY + 230.0f, 16, sf::Color(52, 152, 219), true);
    drawCenteredText(window, "Press ESC to Quit", boxY + 265.0f, 14, sf::Color(149, 165, 166));
}

void UI::renderChallengeGameOverScreen(sf::RenderWindow& window, int wave, int foodEaten, int target,
                                       float gameTime, int windowWidth, int windowHeight) const {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(15, 10, 18, 230));
    window.draw(overlay);

    float boxW = 480.0f;
    float boxH = 320.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = (static_cast<float>(windowHeight) - boxH) / 2.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(26, 18, 28));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(255, 75, 95));
    window.draw(box);

    drawCenteredText(window, "TIME'S UP / GAME OVER", boxY + 38.0f, 30, sf::Color(255, 75, 95), true);
    drawCenteredText(window, "Challenge Attempt Ended", boxY + 74.0f, 16, sf::Color(220, 225, 230));

    drawCenteredText(window, "Wave Reached: Wave " + std::to_string(wave), boxY + 110.0f, 18, sf::Color(255, 215, 0), true);
    drawCenteredText(window, "Food Quota: " + std::to_string(foodEaten) + " / " + std::to_string(target) + " collected",
                     boxY + 140.0f, 16, sf::Color(0, 229, 255));
    drawCenteredText(window, "Total Time Active: " + formatTime(gameTime), boxY + 170.0f, 16, sf::Color(189, 195, 199));

    drawCenteredText(window, "Press ENTER to Retry Wave", boxY + 215.0f, 19, sf::Color(46, 204, 113), true);
    drawCenteredText(window, "Press M for Mode Select", boxY + 248.0f, 16, sf::Color(52, 152, 219), true);
    drawCenteredText(window, "Press ESC to Quit", boxY + 280.0f, 13, sf::Color(149, 165, 166));
}

void UI::renderWinScreen(sf::RenderWindow& window, int score, float gameTime,
                         int windowWidth, int windowHeight) const {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(8, 12, 20, 230));
    window.draw(overlay);

    float boxW = 460.0f;
    float boxH = 290.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = (static_cast<float>(windowHeight) - boxH) / 2.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(18, 26, 22));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(241, 196, 15));
    window.draw(box);

    drawCenteredText(window, "CONGRATULATIONS!", boxY + 40.0f, 34, sf::Color(241, 196, 15), true);
    drawCenteredText(window, "You conquered the entire board!", boxY + 85.0f, 18, sf::Color(46, 204, 113));
    drawCenteredText(window, "Total Score: " + std::to_string(score), boxY + 122.0f, 19, sf::Color(236, 240, 241));
    drawCenteredText(window, "Clear Time: " + formatTime(gameTime), boxY + 155.0f, 17, sf::Color(0, 229, 255));
    drawCenteredText(window, "Press ENTER to Play Again", boxY + 198.0f, 19, sf::Color(52, 152, 219), true);
    drawCenteredText(window, "Press M for Mode Select", boxY + 230.0f, 16, sf::Color(46, 204, 113), true);
    drawCenteredText(window, "Press ESC to Quit", boxY + 260.0f, 14, sf::Color(149, 165, 166));
}

void UI::renderChallengeWinScreen(sf::RenderWindow& window, int wave, int score,
                                  float gameTime, int windowWidth, int windowHeight) const {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(12, 8, 22, 235));
    window.draw(overlay);

    float boxW = 480.0f;
    float boxH = 290.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = (static_cast<float>(windowHeight) - boxH) / 2.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(28, 20, 36));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(255, 105, 180));
    window.draw(box);

    drawCenteredText(window, "CHALLENGE VICTORY!", boxY + 40.0f, 32, sf::Color(255, 105, 180), true);
    drawCenteredText(window, "All Target Food Quotas Conquered!", boxY + 85.0f, 17, sf::Color(46, 204, 113));
    drawCenteredText(window, "Waves Cleared: " + std::to_string(wave), boxY + 120.0f, 18, sf::Color(255, 215, 0));
    drawCenteredText(window, "Total Score: " + std::to_string(score) + "  |  Time: " + formatTime(gameTime), boxY + 152.0f, 16, sf::Color(0, 229, 255));
    drawCenteredText(window, "Press ENTER to Play Again", boxY + 195.0f, 19, sf::Color(52, 152, 219), true);
    drawCenteredText(window, "Press M for Mode Select", boxY + 228.0f, 16, sf::Color(46, 204, 113), true);
    drawCenteredText(window, "Press ESC to Quit", boxY + 258.0f, 14, sf::Color(149, 165, 166));
}

void UI::renderChallengeHUD(sf::RenderWindow& window, int score, int highScore,
                            int foodEaten, int target, float timeLeft, int wave,
                            int windowWidth, int hudHeight) const {
    // 1. Full HUD bar background
    sf::RectangleShape hudBar(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(hudHeight)));
    hudBar.setPosition(0.0f, 0.0f);
    hudBar.setFillColor(sf::Color(12, 15, 22));
    window.draw(hudBar);

    // Separator line at bottom (neon magenta/pink for challenge mode)
    sf::RectangleShape separator(sf::Vector2f(static_cast<float>(windowWidth), 2.0f));
    separator.setPosition(0.0f, static_cast<float>(hudHeight) - 2.0f);
    separator.setFillColor(sf::Color(255, 105, 180, 210));
    window.draw(separator);

    float textY = static_cast<float>(hudHeight) / 2.0f;

    if (m_fontLoaded) {
        // 1. WAVE BADGE CARD (x: 10, w: 85)
        sf::RectangleShape waveCard(sf::Vector2f(85.0f, 26.0f));
        waveCard.setOrigin(0.0f, 13.0f);
        waveCard.setPosition(10.0f, textY);
        waveCard.setFillColor(sf::Color(32, 24, 40));
        waveCard.setOutlineThickness(1.0f);
        waveCard.setOutlineColor(sf::Color(255, 105, 180, 160));
        window.draw(waveCard);

        sf::Text waveText;
        waveText.setFont(m_font);
        waveText.setString("WAVE " + std::to_string(wave));
        waveText.setCharacterSize(13);
        waveText.setStyle(sf::Text::Bold);
        waveText.setFillColor(sf::Color(255, 215, 0));
        sf::FloatRect bW = waveText.getLocalBounds();
        waveText.setOrigin(bW.left + bW.width / 2.0f, bW.top + bW.height / 2.0f);
        waveText.setPosition(10.0f + 42.5f, textY);
        window.draw(waveText);

        // 2. FOOD GOAL & PROGRESS BAR CARD (x: 103, w: 220)
        sf::RectangleShape goalCard(sf::Vector2f(220.0f, 26.0f));
        goalCard.setOrigin(0.0f, 13.0f);
        goalCard.setPosition(103.0f, textY);
        goalCard.setFillColor(sf::Color(18, 26, 36));
        goalCard.setOutlineThickness(1.0f);
        goalCard.setOutlineColor(sf::Color(42, 65, 85));
        window.draw(goalCard);

        sf::Text foodText;
        foodText.setFont(m_font);
        foodText.setString("GOAL: " + std::to_string(foodEaten) + "/" + std::to_string(target));
        foodText.setCharacterSize(12);
        foodText.setStyle(sf::Text::Bold);
        foodText.setFillColor(sf::Color(0, 229, 255));
        sf::FloatRect bF = foodText.getLocalBounds();
        foodText.setOrigin(0.0f, bF.top + bF.height / 2.0f);
        foodText.setPosition(113.0f, textY);
        window.draw(foodText);

        // Progress bar inside goal card
        float progress = (target > 0) ? (static_cast<float>(foodEaten) / static_cast<float>(target)) : 0.0f;
        progress = std::clamp(progress, 0.0f, 1.0f);
        float pBarX = 212.0f;
        float pBarW = 100.0f;
        sf::RectangleShape pBg(sf::Vector2f(pBarW, 8.0f));
        pBg.setPosition(pBarX, textY - 4.0f);
        pBg.setFillColor(sf::Color(28, 36, 48));
        window.draw(pBg);

        if (progress > 0.0f) {
            sf::RectangleShape pFill(sf::Vector2f(pBarW * progress, 8.0f));
            pFill.setPosition(pBarX, textY - 4.0f);
            pFill.setFillColor(sf::Color(46, 204, 113));
            window.draw(pFill);
        }

        // 3. SCORE CARD (x: 331, w: 140)
        sf::RectangleShape scoreCard(sf::Vector2f(140.0f, 26.0f));
        scoreCard.setOrigin(0.0f, 13.0f);
        scoreCard.setPosition(331.0f, textY);
        scoreCard.setFillColor(sf::Color(22, 28, 40));
        scoreCard.setOutlineThickness(1.0f);
        scoreCard.setOutlineColor(sf::Color(40, 52, 72));
        window.draw(scoreCard);

        sf::Text scoreText;
        scoreText.setFont(m_font);
        scoreText.setString("SCORE: " + std::to_string(score));
        scoreText.setCharacterSize(13);
        scoreText.setStyle(sf::Text::Bold);
        scoreText.setFillColor(sf::Color(236, 240, 241));
        sf::FloatRect bS = scoreText.getLocalBounds();
        scoreText.setOrigin(bS.left + bS.width / 2.0f, bS.top + bS.height / 2.0f);
        scoreText.setPosition(331.0f + 70.0f, textY);
        window.draw(scoreText);

        // 4. HIGH SCORE CARD (x: 479, w: 140)
        sf::RectangleShape highCard(sf::Vector2f(140.0f, 26.0f));
        highCard.setOrigin(0.0f, 13.0f);
        highCard.setPosition(479.0f, textY);
        highCard.setFillColor(sf::Color(26, 24, 32));
        highCard.setOutlineThickness(1.0f);
        highCard.setOutlineColor(sf::Color(65, 55, 30));
        window.draw(highCard);

        sf::Text highText;
        highText.setFont(m_font);
        highText.setString("HIGH: " + std::to_string(highScore));
        highText.setCharacterSize(13);
        highText.setStyle(sf::Text::Bold);
        highText.setFillColor(sf::Color(241, 196, 15));
        sf::FloatRect bH = highText.getLocalBounds();
        highText.setOrigin(bH.left + bH.width / 2.0f, bH.top + bH.height / 2.0f);
        highText.setPosition(479.0f + 70.0f, textY);
        window.draw(highText);

        // 5. COUNTDOWN TIMER CARD (x: 627, w: 163)
        sf::Color timeBorder = (timeLeft <= 5.0f) ? sf::Color(255, 45, 75)
                             : (timeLeft <= 10.0f) ? sf::Color(255, 140, 0)
                             : sf::Color(42, 70, 95);
        sf::Color timeBg = (timeLeft <= 5.0f) ? sf::Color(45, 18, 25)
                         : (timeLeft <= 10.0f) ? sf::Color(40, 28, 18)
                         : sf::Color(20, 30, 42);
        sf::Color timeTextCol = (timeLeft <= 5.0f) ? sf::Color(255, 60, 80)
                              : (timeLeft <= 10.0f) ? sf::Color(255, 175, 25)
                              : sf::Color(0, 229, 255);

        sf::RectangleShape timerCard(sf::Vector2f(163.0f, 26.0f));
        timerCard.setOrigin(0.0f, 13.0f);
        timerCard.setPosition(627.0f, textY);
        timerCard.setFillColor(timeBg);
        timerCard.setOutlineThickness(1.2f);
        timerCard.setOutlineColor(timeBorder);
        window.draw(timerCard);

        sf::Text timerText;
        timerText.setFont(m_font);
        char buf[32];
        std::snprintf(buf, sizeof(buf), "TIME: %02.1fs", std::max(0.0f, timeLeft));
        timerText.setString(buf);
        timerText.setCharacterSize(13);
        timerText.setStyle(sf::Text::Bold);
        timerText.setFillColor(timeTextCol);
        sf::FloatRect bT = timerText.getLocalBounds();
        timerText.setOrigin(bT.left + bT.width / 2.0f, bT.top + bT.height / 2.0f);
        timerText.setPosition(627.0f + 81.5f, textY);
        window.draw(timerText);
    } else {
        renderFallbackText(window, "WAVE: " + std::to_string(wave), 15.0f, 10.0f, 14.0f, sf::Color(255, 215, 0));
        renderFallbackText(window, "GOAL: " + std::to_string(foodEaten) + "/" + std::to_string(target), 120.0f, 10.0f, 14.0f, sf::Color(0, 229, 255));
        renderFallbackText(window, "SCORE: " + std::to_string(score), 320.0f, 10.0f, 14.0f, sf::Color(236, 240, 241));
        char buf[32];
        std::snprintf(buf, sizeof(buf), "TIME: %02.1fs", std::max(0.0f, timeLeft));
        renderFallbackText(window, buf, static_cast<float>(windowWidth) - 160.0f, 10.0f, 14.0f, sf::Color(255, 75, 95));
    }
}

void UI::renderFallbackText(sf::RenderWindow& window, const std::string& str,
                            float x, float y, float size, sf::Color color) const {
    float charW = size * 0.55f;
    float curX = x;

    for (char c : str) {
        if (c == ' ') {
            curX += charW;
            continue;
        }

        sf::RectangleShape chRect(sf::Vector2f(charW * 0.8f, size * 0.85f));
        chRect.setPosition(curX, y);
        chRect.setFillColor(sf::Color(color.r, color.g, color.b, 140));
        chRect.setOutlineThickness(1.0f);
        chRect.setOutlineColor(color);
        window.draw(chRect);

        curX += charW;
    }
}
