#include "UI.hpp"
#include <iostream>
#include <vector>
#include <cmath>

UI::UI()
    : m_fontLoaded(false) {
    loadFont("assets/fonts/font.ttf");
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

    // Subtle dark playing field background
    sf::RectangleShape playArea(sf::Vector2f(width, height));
    playArea.setPosition(left, top);
    playArea.setFillColor(sf::Color(20, 24, 34)); // Dark Navy Charcoal
    window.draw(playArea);

    // Subtle grid lines
    sf::Color gridColor(28, 34, 48); // Low-contrast grid line color

    // Vertical lines
    for (int x = minX; x <= maxX + 1; ++x) {
        sf::RectangleShape line(sf::Vector2f(1.0f, height));
        line.setPosition(static_cast<float>(x * cellSize), top);
        line.setFillColor(gridColor);
        window.draw(line);
    }

    // Horizontal lines
    for (int y = minY; y <= maxY + 1; ++y) {
        sf::RectangleShape line(sf::Vector2f(width, 1.0f));
        line.setPosition(left, static_cast<float>(y * cellSize));
        line.setFillColor(gridColor);
        window.draw(line);
    }

    // Outer arena border
    sf::RectangleShape border(sf::Vector2f(width, height));
    border.setPosition(left, top);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(2.0f);
    border.setOutlineColor(sf::Color(52, 73, 94));
    window.draw(border);
}

void UI::renderHUD(sf::RenderWindow& window, int score, int highScore, int level,
                   int windowWidth, int hudHeight) const {
    // Top banner panel
    sf::RectangleShape hudBar(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(hudHeight)));
    hudBar.setPosition(0.0f, 0.0f);
    hudBar.setFillColor(sf::Color(14, 17, 24));
    window.draw(hudBar);

    // Separator accent line below HUD
    sf::RectangleShape separator(sf::Vector2f(static_cast<float>(windowWidth), 2.0f));
    separator.setPosition(0.0f, static_cast<float>(hudHeight) - 2.0f);
    separator.setFillColor(sf::Color(46, 204, 113, 160)); // Emerald green accent
    window.draw(separator);

    float textY = static_cast<float>(hudHeight) / 2.0f;

    if (m_fontLoaded) {
        // Current Score (Left)
        sf::Text scoreText;
        scoreText.setFont(m_font);
        scoreText.setString("SCORE: " + std::to_string(score));
        scoreText.setCharacterSize(18);
        scoreText.setFillColor(sf::Color(236, 240, 241));
        sf::FloatRect bScore = scoreText.getLocalBounds();
        scoreText.setOrigin(0.0f, bScore.top + bScore.height / 2.0f);
        scoreText.setPosition(25.0f, textY);
        window.draw(scoreText);

        // High Score (Center)
        sf::Text highText;
        highText.setFont(m_font);
        highText.setString("HIGH SCORE: " + std::to_string(highScore));
        highText.setCharacterSize(18);
        highText.setFillColor(sf::Color(241, 196, 15)); // Gold
        sf::FloatRect bHigh = highText.getLocalBounds();
        highText.setOrigin(bHigh.left + bHigh.width / 2.0f, bHigh.top + bHigh.height / 2.0f);
        highText.setPosition(static_cast<float>(windowWidth) / 2.0f, textY);
        window.draw(highText);

        // Speed / Level (Right)
        sf::Text levelText;
        levelText.setFont(m_font);
        levelText.setString("SPEED: LVL " + std::to_string(level));
        levelText.setCharacterSize(18);
        levelText.setFillColor(sf::Color(46, 204, 113)); // Green
        sf::FloatRect bLvl = levelText.getLocalBounds();
        levelText.setOrigin(bLvl.width, bLvl.top + bLvl.height / 2.0f);
        levelText.setPosition(static_cast<float>(windowWidth) - 25.0f, textY);
        window.draw(levelText);
    } else {
        renderFallbackText(window, "SCORE: " + std::to_string(score), 25.0f, 10.0f, 16.0f, sf::Color(236, 240, 241));
        renderFallbackText(window, "HI: " + std::to_string(highScore), static_cast<float>(windowWidth) / 2.0f - 50.0f, 10.0f, 16.0f, sf::Color(241, 196, 15));
        renderFallbackText(window, "LVL: " + std::to_string(level), static_cast<float>(windowWidth) - 100.0f, 10.0f, 16.0f, sf::Color(46, 204, 113));
    }
}

void UI::renderStartScreen(sf::RenderWindow& window, int windowWidth, int windowHeight, float blinkTimer) const {
    // Backdrop overlay
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(12, 15, 22, 235));
    window.draw(overlay);

    // Title Card
    drawCenteredText(window, "SNAKE", 140.0f, 64, sf::Color(46, 204, 113), true);
    drawCenteredText(window, "2D ARCADE EDITION", 195.0f, 18, sf::Color(149, 165, 166));

    // Instructions Box
    float boxW = 440.0f;
    float boxH = 170.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = 240.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(20, 26, 38));
    box.setOutlineThickness(1.5f);
    box.setOutlineColor(sf::Color(52, 73, 94));
    window.draw(box);

    drawCenteredText(window, "CONTROLS", boxY + 25.0f, 18, sf::Color(241, 196, 15), true);
    drawCenteredText(window, "W, A, S, D  or  Arrow Keys  :  Move Snake", boxY + 60.0f, 16, sf::Color(220, 225, 230));
    drawCenteredText(window, "SPACE  :  Pause / Resume", boxY + 95.0f, 16, sf::Color(220, 225, 230));
    drawCenteredText(window, "ESCAPE  :  Exit Game", boxY + 130.0f, 16, sf::Color(220, 225, 230));

    // Blinking "Press ENTER to Start" prompt
    bool showPrompt = std::fmod(blinkTimer, 1.0f) < 0.65f;
    if (showPrompt) {
        drawCenteredText(window, "PRESS ENTER TO START", 470.0f, 22, sf::Color(52, 152, 219), true);
    }
}

void UI::renderPauseScreen(sf::RenderWindow& window, int windowWidth, int windowHeight) const {
    // Translucent dark overlay
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(8, 10, 16, 190));
    window.draw(overlay);

    float boxW = 340.0f;
    float boxH = 180.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = (static_cast<float>(windowHeight) - boxH) / 2.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(22, 28, 42));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(52, 152, 219));
    window.draw(box);

    drawCenteredText(window, "PAUSED", boxY + 45.0f, 36, sf::Color(241, 196, 15), true);
    drawCenteredText(window, "Press SPACE to Resume", boxY + 105.0f, 18, sf::Color(236, 240, 241));
    drawCenteredText(window, "Press ESC to Quit", boxY + 140.0f, 15, sf::Color(149, 165, 166));
}

void UI::renderGameOverScreen(sf::RenderWindow& window, int score, int highScore,
                              int windowWidth, int windowHeight) const {
    // Dark overlay
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(12, 14, 20, 215));
    window.draw(overlay);

    float boxW = 420.0f;
    float boxH = 260.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = (static_cast<float>(windowHeight) - boxH) / 2.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(24, 20, 26));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(231, 76, 60)); // Crimson
    window.draw(box);

    drawCenteredText(window, "GAME OVER", boxY + 45.0f, 38, sf::Color(231, 76, 60), true);

    if (score >= highScore && score > 0) {
        drawCenteredText(window, "* NEW HIGH SCORE! *", boxY + 95.0f, 20, sf::Color(241, 196, 15), true);
    } else {
        drawCenteredText(window, "Final Score: " + std::to_string(score), boxY + 95.0f, 20, sf::Color(236, 240, 241));
    }

    drawCenteredText(window, "Best Score: " + std::to_string(highScore), boxY + 135.0f, 18, sf::Color(189, 195, 199));
    drawCenteredText(window, "Press ENTER to Restart", boxY + 185.0f, 20, sf::Color(46, 204, 113), true);
    drawCenteredText(window, "Press ESC to Quit", boxY + 225.0f, 15, sf::Color(149, 165, 166));
}

void UI::renderWinScreen(sf::RenderWindow& window, int score,
                         int windowWidth, int windowHeight) const {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    overlay.setFillColor(sf::Color(10, 15, 25, 225));
    window.draw(overlay);

    float boxW = 440.0f;
    float boxH = 260.0f;
    float boxX = (static_cast<float>(windowWidth) - boxW) / 2.0f;
    float boxY = (static_cast<float>(windowHeight) - boxH) / 2.0f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(20, 28, 24));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(241, 196, 15)); // Gold
    window.draw(box);

    drawCenteredText(window, "CONGRATULATIONS!", boxY + 45.0f, 34, sf::Color(241, 196, 15), true);
    drawCenteredText(window, "You conquered the entire board!", boxY + 95.0f, 18, sf::Color(46, 204, 113));
    drawCenteredText(window, "Total Score: " + std::to_string(score), boxY + 135.0f, 20, sf::Color(236, 240, 241));
    drawCenteredText(window, "Press ENTER to Play Again", boxY + 185.0f, 20, sf::Color(52, 152, 219), true);
    drawCenteredText(window, "Press ESC to Quit", boxY + 225.0f, 15, sf::Color(149, 165, 166));
}

void UI::renderFallbackText(sf::RenderWindow& window, const std::string& str,
                            float x, float y, float size, sf::Color color) const {
    // Simple 5x7 vector dot/segment fallback for when no system TTF font is accessible
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

