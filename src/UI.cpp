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
    drawCenteredText(window, "Danger  :  Only biting your own body will kill you!", boxY + 174.0f, 14, sf::Color(231, 76, 60));

    bool showPrompt = std::fmod(blinkTimer, 1.0f) < 0.65f;
    if (showPrompt) {
        drawCenteredText(window, "PRESS ENTER TO START", 480.0f, 22, sf::Color(46, 204, 113), true);
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

    float boxW = 440.0f;
    float boxH = 290.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = (static_cast<float>(windowHeight) - boxH) / 2.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(22, 18, 24));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(231, 76, 60));
    window.draw(box);

    drawCenteredText(window, "GAME OVER", boxY + 42.0f, 36, sf::Color(231, 76, 60), true);

    if (score >= highScore && score > 0) {
        drawCenteredText(window, "* NEW HIGH SCORE! *", boxY + 92.0f, 20, sf::Color(241, 196, 15), true);
    } else {
        drawCenteredText(window, "Final Score: " + std::to_string(score), boxY + 92.0f, 19, sf::Color(236, 240, 241));
    }

    drawCenteredText(window, "Time Survived: " + formatTime(gameTime), boxY + 130.0f, 17, sf::Color(0, 229, 255));
    drawCenteredText(window, "Best Score: " + std::to_string(highScore), boxY + 165.0f, 16, sf::Color(189, 195, 199));
    drawCenteredText(window, "Press ENTER to Restart", boxY + 212.0f, 20, sf::Color(46, 204, 113), true);
    drawCenteredText(window, "Press ESC to Quit", boxY + 252.0f, 14, sf::Color(149, 165, 166));
}

void UI::renderWinScreen(sf::RenderWindow& window, int score, float gameTime,
                         int windowWidth, int windowHeight) const {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(8, 12, 20, 230));
    window.draw(overlay);

    float boxW = 460.0f;
    float boxH = 280.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = (static_cast<float>(windowHeight) - boxH) / 2.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(18, 26, 22));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(241, 196, 15));
    window.draw(box);

    drawCenteredText(window, "CONGRATULATIONS!", boxY + 45.0f, 34, sf::Color(241, 196, 15), true);
    drawCenteredText(window, "You conquered the entire board!", boxY + 95.0f, 18, sf::Color(46, 204, 113));
    drawCenteredText(window, "Total Score: " + std::to_string(score), boxY + 135.0f, 19, sf::Color(236, 240, 241));
    drawCenteredText(window, "Clear Time: " + formatTime(gameTime), boxY + 168.0f, 17, sf::Color(0, 229, 255));
    drawCenteredText(window, "Press ENTER to Play Again", boxY + 215.0f, 20, sf::Color(52, 152, 219), true);
    drawCenteredText(window, "Press ESC to Quit", boxY + 250.0f, 14, sf::Color(149, 165, 166));
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
