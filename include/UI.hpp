#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class UI {
public:
    UI();

    bool loadFont(const std::string& preferredPath);

    // Draws arena grid, subtle dots, and glowing cyan portal borders
    void renderGrid(sf::RenderWindow& window,
                    int minX, int maxX, int minY, int maxY, int cellSize) const;

    // Draws high-tech HUD bar with Score, Timer (MM:SS), High Score, and Level progress
    void renderHUD(sf::RenderWindow& window, int score, int highScore, int level, float gameTime,
                   int windowWidth, int hudHeight) const;

    // Screens
    void renderModeSelectScreen(sf::RenderWindow& window, int windowWidth, int windowHeight, float blinkTimer) const;
    void renderStartScreen(sf::RenderWindow& window, int windowWidth, int windowHeight, float blinkTimer) const;
    void renderPauseScreen(sf::RenderWindow& window, int windowWidth, int windowHeight) const;
    void renderGameOverScreen(sf::RenderWindow& window, int score, int highScore, float gameTime,
                              int windowWidth, int windowHeight) const;
    void renderChallengeGameOverScreen(sf::RenderWindow& window, int wave, int foodEaten, int target,
                                       float gameTime, int windowWidth, int windowHeight) const;
    void renderWinScreen(sf::RenderWindow& window, int score, float gameTime,
                         int windowWidth, int windowHeight) const;
    void renderChallengeWinScreen(sf::RenderWindow& window, int wave, int score,
                                  float gameTime, int windowWidth, int windowHeight) const;

    // Challenge Mode top HUD (Wave, Target quota progress, Score, High score, and Countdown timer)
    void renderChallengeHUD(sf::RenderWindow& window, int score, int highScore,
                            int foodEaten, int target, float timeLeft, int wave,
                            int windowWidth, int hudHeight) const;

    // Helper to draw floating score/level texts
    void renderFloatingText(sf::RenderWindow& window, const std::string& text,
                            const sf::Vector2f& pos, sf::Color color, float alpha) const;

private:
    void drawCenteredText(sf::RenderWindow& window, const std::string& textStr,
                          float centerY, unsigned int charSize, sf::Color color,
                          bool isBold = false) const;

    void renderFallbackText(sf::RenderWindow& window, const std::string& str,
                            float x, float y, float size, sf::Color color) const;

    static std::string formatTime(float seconds);

    sf::Font m_font;
    bool m_fontLoaded;
};
