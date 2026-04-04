#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <SFML/Graphics.hpp>
#include "QuestionManager.h"
#include <string>

class UIManager {
public:
    UIManager();

    bool loadFont(const std::string& filepath);
    bool loadUIAssets();

    // HUD
    void drawHUD(sf::RenderWindow& window, int hp, int maxHp, int xp, int level,
                 int score, int wave, int maxWaves);

    // Menus
    void drawMainMenu(sf::RenderWindow& window);
    void drawInstructions(sf::RenderWindow& window);
    void drawGameOver(sf::RenderWindow& window, int score);
    void drawWin(sf::RenderWindow& window, int score, int level);

    // Battle UI
    void drawBattleOverlay(sf::RenderWindow& window);
    void drawQuestionPanel(sf::RenderWindow& window, const Question& q,
                           int comboStreak, float comboMultiplier,
                           float timerRatio, int turnCount);
    void drawFeedback(sf::RenderWindow& window, bool correct, char correctAnswer,
                      int damageDealt, int comboStreak, bool timedOut);

    // World UI
    void drawEnemyHPBar(sf::RenderWindow& window, float x, float y,
                        int hp, int maxHp);

    void drawPauseMenu(sf::RenderWindow& window);
    void drawShopPanel(sf::RenderWindow& window, int playerScore, int selection);

    sf::Font* getFont();

    int menuSelection = 0;
    bool showInstructions = false;

private:
    sf::Font font;
    bool fontLoaded = false;

    // UI asset textures
    sf::Texture panelTexture;
    sf::Texture panelInsetTexture;
    sf::Texture buttonTexture;
    sf::Texture buttonPressedTexture;
    sf::Texture buttonBeige;
    sf::Texture buttonBeigePressed;
    sf::Texture barBackLeft, barBackMid, barBackRight;
    sf::Texture barGreenLeft, barGreenMid, barGreenRight;
    sf::Texture barYellowLeft, barYellowMid, barYellowRight;
    sf::Texture barRedLeft, barRedMid, barRedRight;
    sf::Texture iconCheck;
    sf::Texture iconCross;
    sf::Texture logoTexture;
    bool logoLoaded = false;
    bool uiAssetsLoaded = false;

    // Helper: draw a 9-slice scaled panel
    void drawPanel(sf::RenderWindow& window, sf::Texture& tex, float x, float y,
                   float w, float h);
    // Helper: draw a bar using left/mid/right pieces
    void drawBar(sf::RenderWindow& window,
                 sf::Texture& leftTex, sf::Texture& midTex, sf::Texture& rightTex,
                 float x, float y, float totalWidth, float fillRatio, float height);
};

#endif
