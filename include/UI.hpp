#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class UI {
public:
    UI();

    // Loads the font with multiple fallback searches
    bool loadFont(const std::string& preferredPath);

    // Draws the playing field background, subtle grid, and border
    void renderGrid(sf::RenderWindow& window,
                    int minX, int maxX, int minY, int maxY, int cellSize) const;

    // Draws top HUD banner showing current score, high score, and level
    void renderHUD(sf::RenderWindow& window, int score, int highScore, int level,
                   int windowWidth, int hudHeight) const;

    // Draws start menu overlay
    void renderStartScreen(sf::RenderWindow& window, int windowWidth, int windowHeight, float blinkTimer) const;

    // Draws paused overlay
    void renderPauseScreen(sf::RenderWindow& window, int windowWidth, int windowHeight) const;

    // Draws game over card with scores and restart prompt
    void renderGameOverScreen(sf::RenderWindow& window, int score, int highScore,
                              int windowWidth, int windowHeight) const;

    // Draws victory card when grid is full
    void renderWinScreen(sf::RenderWindow& window, int score,
                         int windowWidth, int windowHeight) const;

private:
    // Helper to draw text centered horizontally
    void drawCenteredText(sf::RenderWindow& window, const std::string& textStr,
                          float centerY, unsigned int charSize, sf::Color color,
                          bool isBold = false) const;

    // Fallback geometric character renderer in case system has no TTF fonts
    void renderFallbackText(sf::RenderWindow& window, const std::string& str,
                            float x, float y, float size, sf::Color color) const;

    sf::Font m_font;
    bool m_fontLoaded;
};

