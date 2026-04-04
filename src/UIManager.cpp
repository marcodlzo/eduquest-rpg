#include "UIManager.h"
#include <cmath>

UIManager::UIManager() {}

sf::Font* UIManager::getFont() {
    return fontLoaded ? &font : nullptr;
}

bool UIManager::loadFont(const std::string& filepath) {
    fontLoaded = font.loadFromFile(filepath);
    if (!fontLoaded)
        std::printf("Warning: could not load font from %s\n", filepath.c_str());
    return fontLoaded;
}

bool UIManager::loadUIAssets() {
    bool ok = true;
    ok &= panelTexture.loadFromFile("assets/images/ui/panel_blue.png");
    ok &= panelInsetTexture.loadFromFile("assets/images/ui/panelInset_brown.png");
    ok &= buttonTexture.loadFromFile("assets/images/ui/buttonLong_blue.png");
    ok &= buttonPressedTexture.loadFromFile("assets/images/ui/buttonLong_blue_pressed.png");
    ok &= buttonBeige.loadFromFile("assets/images/ui/buttonLong_beige.png");
    ok &= buttonBeigePressed.loadFromFile("assets/images/ui/buttonLong_beige_pressed.png");

    ok &= barBackLeft.loadFromFile("assets/images/ui/barBack_horizontalLeft.png");
    ok &= barBackMid.loadFromFile("assets/images/ui/barBack_horizontalMid.png");
    ok &= barBackRight.loadFromFile("assets/images/ui/barBack_horizontalRight.png");
    ok &= barGreenLeft.loadFromFile("assets/images/ui/barGreen_horizontalLeft.png");
    ok &= barGreenMid.loadFromFile("assets/images/ui/barGreen_horizontalMid.png");
    ok &= barGreenRight.loadFromFile("assets/images/ui/barGreen_horizontalRight.png");
    ok &= barYellowLeft.loadFromFile("assets/images/ui/barYellow_horizontalLeft.png");
    ok &= barYellowMid.loadFromFile("assets/images/ui/barYellow_horizontalMid.png");
    ok &= barYellowRight.loadFromFile("assets/images/ui/barYellow_horizontalRight.png");
    ok &= barRedLeft.loadFromFile("assets/images/ui/barRed_horizontalLeft.png");
    ok &= barRedMid.loadFromFile("assets/images/ui/barRed_horizontalMid.png");
    ok &= barRedRight.loadFromFile("assets/images/ui/barRed_horizontalRight.png");

    ok &= iconCheck.loadFromFile("assets/images/ui/iconCheck_blue.png");
    ok &= iconCross.loadFromFile("assets/images/ui/iconCross_brown.png");

    if (!ok)
        std::printf("Warning: some UI assets failed to load\n");

    logoLoaded = logoTexture.loadFromFile("assets/images/logo/Blue and Green Illustrative Game Night Presentation (1).png");
    if (!logoLoaded)
        std::printf("Warning: could not load logo image\n");

    uiAssetsLoaded = ok;
    return ok;
}

void UIManager::drawPanel(sf::RenderWindow& window, sf::Texture& tex,
                           float x, float y, float w, float h) {
    sf::Sprite panel;
    panel.setTexture(tex);
    sf::Vector2u size = tex.getSize();
    panel.setScale(w / size.x, h / size.y);
    panel.setPosition(x, y);
    window.draw(panel);
}

void UIManager::drawBar(sf::RenderWindow& window,
                         sf::Texture& leftTex, sf::Texture& midTex, sf::Texture& rightTex,
                         float x, float y, float totalWidth, float fillRatio, float height) {
    if (fillRatio < 0.f) fillRatio = 0.f;
    if (fillRatio > 1.f) fillRatio = 1.f;

    float scaleY = height / 18.f;
    float leftW = 9.f * scaleY;
    float rightW = 9.f * scaleY;
    float midW = totalWidth - leftW - rightW;
    float fillW = totalWidth * fillRatio;

    sf::Sprite left;
    left.setTexture(leftTex);
    left.setScale(scaleY, scaleY);
    left.setPosition(x, y);
    window.draw(left);

    if (fillW > leftW) {
        sf::Sprite mid;
        mid.setTexture(midTex);
        float actualMidW = std::min(fillW - leftW, midW);
        mid.setTextureRect(sf::IntRect(0, 0, (int)(actualMidW / scaleY), 18));
        mid.setScale(scaleY, scaleY);
        mid.setPosition(x + leftW, y);
        window.draw(mid);
    }

    if (fillW >= totalWidth - rightW) {
        sf::Sprite right;
        right.setTexture(rightTex);
        right.setScale(scaleY, scaleY);
        right.setPosition(x + totalWidth - rightW, y);
        window.draw(right);
    }
}

void UIManager::drawHUD(sf::RenderWindow& window, int hp, int maxHp, int xp,
                         int level, int score, int wave, int maxWaves) {
    float barWidth = 200.f;
    float barHeight = 22.f;

    if (uiAssetsLoaded) {
        drawBar(window, barRedLeft, barRedMid, barRedRight,
                10.f, 10.f, barWidth, 1.f, barHeight);
        float hpRatio = (maxHp > 0) ? static_cast<float>(hp) / maxHp : 0.f;
        drawBar(window, barGreenLeft, barGreenMid, barGreenRight,
                10.f, 10.f, barWidth, hpRatio, barHeight);

        drawBar(window, barBackLeft, barBackMid, barBackRight,
                10.f, 37.f, barWidth, 1.f, 16.f);
        int xpForNextLevel = level * 50;
        float xpRatio = (xpForNextLevel > 0) ? static_cast<float>(xp) / xpForNextLevel : 0.f;
        if (xpRatio > 1.f) xpRatio = 1.f;
        drawBar(window, barYellowLeft, barYellowMid, barYellowRight,
                10.f, 37.f, barWidth, xpRatio, 16.f);
    } else {
        sf::RectangleShape hpBg(sf::Vector2f(barWidth, 20.f));
        hpBg.setPosition(10.f, 10.f);
        hpBg.setFillColor(sf::Color(180, 30, 30));
        window.draw(hpBg);

        float hpRatio = (maxHp > 0) ? static_cast<float>(hp) / maxHp : 0.f;
        sf::RectangleShape hpFill(sf::Vector2f(barWidth * hpRatio, 20.f));
        hpFill.setPosition(10.f, 10.f);
        hpFill.setFillColor(sf::Color(30, 180, 30));
        window.draw(hpFill);

        sf::RectangleShape xpBg(sf::Vector2f(barWidth, 12.f));
        xpBg.setPosition(10.f, 35.f);
        xpBg.setFillColor(sf::Color(100, 100, 100));
        window.draw(xpBg);

        int xpForNextLevel = level * 50;
        float xpRatio = (xpForNextLevel > 0) ? static_cast<float>(xp) / xpForNextLevel : 0.f;
        if (xpRatio > 1.f) xpRatio = 1.f;
        sf::RectangleShape xpFill(sf::Vector2f(barWidth * xpRatio, 12.f));
        xpFill.setPosition(10.f, 35.f);
        xpFill.setFillColor(sf::Color(220, 200, 30));
        window.draw(xpFill);
    }

    if (fontLoaded) {
        sf::Text hpText;
        hpText.setFont(font);
        hpText.setCharacterSize(14);
        hpText.setFillColor(sf::Color::White);
        hpText.setString("HP: " + std::to_string(hp) + "/" + std::to_string(maxHp));
        hpText.setPosition(15.f, 10.f);
        window.draw(hpText);

        sf::Text xpText;
        xpText.setFont(font);
        xpText.setCharacterSize(11);
        xpText.setFillColor(sf::Color::White);
        int xpForNextLevel = level * 50;
        xpText.setString("XP: " + std::to_string(xp) + "/" + std::to_string(xpForNextLevel));
        xpText.setPosition(15.f, 36.f);
        window.draw(xpText);

        sf::Text levelText;
        levelText.setFont(font);
        levelText.setCharacterSize(20);
        levelText.setFillColor(sf::Color::White);
        levelText.setString("Level " + std::to_string(level));
        levelText.setPosition(600.f, 10.f);
        window.draw(levelText);

        sf::Text waveText;
        waveText.setFont(font);
        waveText.setCharacterSize(18);
        waveText.setFillColor(sf::Color(255, 215, 0));
        waveText.setString("Wave " + std::to_string(wave) + "/" + std::to_string(maxWaves));
        waveText.setPosition(750.f, 10.f);
        window.draw(waveText);

        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setPosition(1130.f, 10.f);
        window.draw(scoreText);
    }
}

void UIManager::drawMainMenu(sf::RenderWindow& window) {
    // Draw logo as full-screen background
    if (logoLoaded) {
        sf::Sprite logoBg;
        logoBg.setTexture(logoTexture);
        sf::Vector2u texSize = logoTexture.getSize();
        logoBg.setScale(1280.f / texSize.x, 720.f / texSize.y);
        window.draw(logoBg);
    } else {
        window.clear(sf::Color(20, 20, 40));
    }

    if (!fontLoaded) return;

    // Draw horizontal menu items centered below the title
    std::string options[] = {"START", "INSTRUCTIONS", "QUIT"};
    float totalWidth = 0.f;
    float spacing = 40.f;
    float btnPadX = 30.f;
    float btnH = 45.f;
    float menuY = 510.f;

    // Measure total width of all buttons
    std::vector<float> textWidths(3);
    for (int i = 0; i < 3; i++) {
        sf::Text tmp;
        tmp.setFont(font);
        tmp.setCharacterSize(22);
        tmp.setString(options[i]);
        textWidths[i] = tmp.getLocalBounds().width;
        totalWidth += textWidths[i] + btnPadX * 2;
    }
    totalWidth += spacing * 2; // spacing between buttons

    float startX = 640.f - totalWidth / 2.f;
    float curX = startX;

    for (int i = 0; i < 3; i++) {
        float btnW = textWidths[i] + btnPadX * 2;

        if (uiAssetsLoaded) {
            sf::Sprite btn;
            btn.setTexture(i == menuSelection ? buttonTexture : buttonBeige);
            btn.setScale(btnW / 190.f, btnH / 49.f);
            btn.setPosition(curX, menuY);
            window.draw(btn);
        } else {
            sf::RectangleShape btn(sf::Vector2f(btnW, btnH));
            btn.setPosition(curX, menuY);
            btn.setFillColor(i == menuSelection ? sf::Color(60, 100, 180) : sf::Color(80, 80, 100));
            btn.setOutlineColor(i == menuSelection ? sf::Color::Yellow : sf::Color::White);
            btn.setOutlineThickness(2.f);
            window.draw(btn);
        }

        sf::Text option;
        option.setFont(font);
        option.setCharacterSize(22);
        if (uiAssetsLoaded)
            option.setFillColor(i == menuSelection ? sf::Color::White : sf::Color(50, 50, 80));
        else
            option.setFillColor(i == menuSelection ? sf::Color::Yellow : sf::Color::White);
        option.setString(options[i]);
        sf::FloatRect ob = option.getLocalBounds();
        option.setPosition(curX + btnW / 2.f - ob.width / 2.f, menuY + btnH / 2.f - ob.height / 2.f - 4.f);
        window.draw(option);

        // Arrow indicators for selected item
        if (i == menuSelection && fontLoaded) {
            sf::Text arrow;
            arrow.setFont(font);
            arrow.setCharacterSize(22);
            arrow.setFillColor(sf::Color::Yellow);
            if (i > 0) {
                arrow.setString("<");
                arrow.setPosition(curX - 20.f, menuY + 10.f);
                window.draw(arrow);
            }
            if (i < 2) {
                arrow.setString(">");
                arrow.setPosition(curX + btnW + 8.f, menuY + 10.f);
                window.draw(arrow);
            }
        }

        curX += btnW + spacing;
    }

    // Hint text
    sf::Text hint;
    hint.setFont(font);
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setString("Use Left/Right arrows to navigate, Enter to select");
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setPosition(640.f - hb.width / 2.f, 570.f);
    window.draw(hint);
}

void UIManager::drawInstructions(sf::RenderWindow& window) {
    window.clear(sf::Color(20, 20, 40));

    if (!fontLoaded) return;

    if (uiAssetsLoaded) {
        drawPanel(window, panelTexture, 200.f, 40.f, 880.f, 640.f);
    }

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(40);
    title.setFillColor(sf::Color::Yellow);
    title.setString("How to Play");
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition(640.f - tb.width / 2.f, 60.f);
    window.draw(title);

    std::string lines[] = {
        "Move with WASD keys",
        "Press E near an enemy or chest to interact",
        "Press E near the Wizard to open the Shop",
        "Spend score on Health Potions or Power Boosts!",
        "Answer questions by pressing A, B, C, or D",
        "Harder questions deal more damage (15/25/40)",
        "Build combos for damage multipliers (up to x2.5)!",
        "Answer fast for bonus damage! Time runs out = wrong!",
        "Open chests for health potions or score bonuses!",
        "Clear all enemies to advance (3 waves total)",
        "Press ESC to pause, Q to quit to menu",
        "Categories: Programming, Science, Computer Science",
        "",
        "Press Escape to return to menu"
    };

    for (int i = 0; i < 14; i++) {
        sf::Text line;
        line.setFont(font);
        line.setCharacterSize(20);
        line.setFillColor(sf::Color::White);
        line.setString(lines[i]);
        line.setPosition(280.f, 130.f + i * 40.f);
        window.draw(line);
    }
}

void UIManager::drawGameOver(sf::RenderWindow& window, int score) {
    window.clear(sf::Color(60, 10, 10));

    if (!fontLoaded) return;

    if (uiAssetsLoaded) {
        drawPanel(window, panelTexture, 300.f, 150.f, 680.f, 380.f);
    }

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(60);
    title.setFillColor(sf::Color::Red);
    title.setString("GAME OVER");
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition(640.f - tb.width / 2.f, 200.f);
    window.draw(title);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Final Score: " + std::to_string(score));
    sf::FloatRect sb = scoreText.getLocalBounds();
    scoreText.setPosition(640.f - sb.width / 2.f, 330.f);
    window.draw(scoreText);

    sf::Text hint;
    hint.setFont(font);
    hint.setCharacterSize(20);
    hint.setFillColor(sf::Color(150, 150, 150));
    hint.setString("Press Enter to return to menu");
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setPosition(640.f - hb.width / 2.f, 450.f);
    window.draw(hint);
}

void UIManager::drawWin(sf::RenderWindow& window, int score, int level) {
    window.clear(sf::Color(10, 30, 60));

    if (!fontLoaded) return;

    if (uiAssetsLoaded) {
        drawPanel(window, panelTexture, 300.f, 130.f, 680.f, 420.f);
    }

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(60);
    title.setFillColor(sf::Color(255, 215, 0));
    title.setString("YOU WIN!");
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition(640.f - tb.width / 2.f, 170.f);
    window.draw(title);

    sf::Text subtitle;
    subtitle.setFont(font);
    subtitle.setCharacterSize(24);
    subtitle.setFillColor(sf::Color(200, 200, 255));
    subtitle.setString("All 3 waves cleared!");
    sf::FloatRect stb = subtitle.getLocalBounds();
    subtitle.setPosition(640.f - stb.width / 2.f, 260.f);
    window.draw(subtitle);

    sf::Text stats;
    stats.setFont(font);
    stats.setCharacterSize(28);
    stats.setFillColor(sf::Color::White);
    stats.setString("Final Score: " + std::to_string(score) +
                    "\nFinal Level: " + std::to_string(level));
    stats.setPosition(490.f, 320.f);
    window.draw(stats);

    sf::Text hint;
    hint.setFont(font);
    hint.setCharacterSize(20);
    hint.setFillColor(sf::Color(150, 150, 150));
    hint.setString("Press Enter to return to menu");
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setPosition(640.f - hb.width / 2.f, 470.f);
    window.draw(hint);
}

void UIManager::drawBattleOverlay(sf::RenderWindow& window) {
    sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
}

void UIManager::drawQuestionPanel(sf::RenderWindow& window, const Question& q,
                                   int comboStreak, float comboMultiplier,
                                   float timerRatio, int turnCount) {
    // Panel background
    if (uiAssetsLoaded) {
        drawPanel(window, panelTexture, 250.f, 100.f, 780.f, 500.f);
        drawPanel(window, panelInsetTexture, 280.f, 155.f, 720.f, 80.f);
    } else {
        sf::RectangleShape panel(sf::Vector2f(700.f, 460.f));
        panel.setPosition(290.f, 110.f);
        panel.setFillColor(sf::Color(30, 30, 60));
        panel.setOutlineColor(sf::Color::White);
        panel.setOutlineThickness(2.f);
        window.draw(panel);
    }

    if (!fontLoaded) return;

    // Timer bar at top of panel
    float timerBarWidth = 700.f;
    float timeLeft = 1.f - timerRatio;
    sf::Color timerColor;
    if (timeLeft > 0.5f) timerColor = sf::Color(30, 200, 30);
    else if (timeLeft > 0.25f) timerColor = sf::Color(220, 200, 30);
    else timerColor = sf::Color(220, 50, 30);

    sf::RectangleShape timerBg(sf::Vector2f(timerBarWidth, 8.f));
    timerBg.setPosition(290.f, 112.f);
    timerBg.setFillColor(sf::Color(60, 60, 60));
    window.draw(timerBg);

    sf::RectangleShape timerFill(sf::Vector2f(timerBarWidth * timeLeft, 8.f));
    timerFill.setPosition(290.f, 112.f);
    timerFill.setFillColor(timerColor);
    window.draw(timerFill);

    // Turn counter + Combo display
    sf::Text turnText;
    turnText.setFont(font);
    turnText.setCharacterSize(14);
    turnText.setFillColor(sf::Color(180, 180, 200));
    turnText.setString("Turn " + std::to_string(turnCount));
    turnText.setPosition(270.f, 125.f);
    window.draw(turnText);

    if (comboStreak > 0) {
        sf::Text comboText;
        comboText.setFont(font);
        comboText.setCharacterSize(16);

        // Combo color gets more intense
        if (comboStreak >= 4) comboText.setFillColor(sf::Color(255, 50, 50));
        else if (comboStreak >= 3) comboText.setFillColor(sf::Color(255, 150, 0));
        else if (comboStreak >= 2) comboText.setFillColor(sf::Color(255, 215, 0));
        else comboText.setFillColor(sf::Color(200, 200, 255));

        std::string multStr;
        if (comboMultiplier == 1.0f) multStr = "x1";
        else if (comboMultiplier == 1.5f) multStr = "x1.5";
        else if (comboMultiplier == 2.0f) multStr = "x2";
        else multStr = "x2.5";

        comboText.setString("COMBO " + std::to_string(comboStreak) +
                           " (" + multStr + " dmg)");
        sf::FloatRect cb = comboText.getLocalBounds();
        comboText.setPosition(990.f - cb.width, 125.f);
        window.draw(comboText);
    }

    // Category + Difficulty
    sf::Text catText;
    catText.setFont(font);
    catText.setCharacterSize(16);
    catText.setFillColor(sf::Color(150, 150, 200));
    catText.setString("[" + q.category + " - " + q.difficulty + "]");
    catText.setPosition(300.f, 145.f);
    window.draw(catText);

    // Question text
    sf::Text qText;
    qText.setFont(font);
    qText.setCharacterSize(22);
    qText.setFillColor(sf::Color::White);
    qText.setString(q.questionText);
    qText.setPosition(300.f, 170.f);
    window.draw(qText);

    // Options
    std::string options[] = {
        "A) " + q.optionA,
        "B) " + q.optionB,
        "C) " + q.optionC,
        "D) " + q.optionD
    };

    for (int i = 0; i < 4; i++) {
        float optY = 260.f + i * 60.f;

        if (uiAssetsLoaded) {
            sf::Sprite btn;
            btn.setTexture(buttonBeige);
            float btnW = 640.f;
            float btnH = 45.f;
            btn.setScale(btnW / 190.f, btnH / 49.f);
            btn.setPosition(320.f, optY);
            window.draw(btn);
        }

        sf::Text optText;
        optText.setFont(font);
        optText.setCharacterSize(20);
        optText.setFillColor(uiAssetsLoaded ? sf::Color(50, 50, 80) : sf::Color(200, 200, 255));
        optText.setString(options[i]);
        optText.setPosition(340.f, optY + 8.f);
        window.draw(optText);
    }

    sf::Text hint;
    hint.setFont(font);
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(120, 120, 120));
    hint.setString("Press A, B, C, or D to answer");
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setPosition(640.f - hb.width / 2.f, 515.f);
    window.draw(hint);
}

void UIManager::drawFeedback(sf::RenderWindow& window, bool correct,
                              char correctAnswer, int damageDealt,
                              int comboStreak, bool timedOut) {
    drawBattleOverlay(window);

    if (!fontLoaded) return;

    if (uiAssetsLoaded) {
        drawPanel(window, panelTexture, 300.f, 220.f, 680.f, 260.f);
    }

    // Icon
    if (uiAssetsLoaded) {
        sf::Sprite icon;
        icon.setTexture(correct ? iconCheck : iconCross);
        icon.setScale(4.f, 4.f);
        icon.setPosition(520.f, 235.f);
        window.draw(icon);
    }

    // Main feedback text
    sf::Text feedback;
    feedback.setFont(font);
    feedback.setCharacterSize(36);

    if (correct) {
        feedback.setFillColor(sf::Color::Green);
        feedback.setString("CORRECT!");
    } else if (timedOut) {
        feedback.setFillColor(sf::Color(255, 150, 0));
        feedback.setString("TIME'S UP!");
    } else {
        feedback.setFillColor(sf::Color::Red);
        feedback.setString(std::string("WRONG! Answer: ") + correctAnswer);
    }

    sf::FloatRect fb = feedback.getLocalBounds();
    feedback.setPosition(640.f - fb.width / 2.f, 310.f);
    window.draw(feedback);

    // Damage/penalty detail line
    sf::Text detail;
    detail.setFont(font);
    detail.setCharacterSize(22);

    if (correct) {
        std::string dmgStr = std::to_string(damageDealt) + " damage!";
        if (comboStreak >= 2)
            dmgStr += "  COMBO x" + std::to_string(comboStreak) + "!";
        detail.setFillColor(sf::Color(150, 255, 150));
        detail.setString(dmgStr);
    } else {
        detail.setFillColor(sf::Color(255, 150, 150));
        detail.setString("You take 10 damage!");
    }

    sf::FloatRect db = detail.getLocalBounds();
    detail.setPosition(640.f - db.width / 2.f, 370.f);
    window.draw(detail);
}

void UIManager::drawEnemyHPBar(sf::RenderWindow& window, float x, float y,
                                int hp, int maxHp) {
    float barW = 40.f;
    float barH = 5.f;
    float barX = x + 4.f; // center above 48px sprite
    float barY = y - 8.f;

    // Background
    sf::RectangleShape bg(sf::Vector2f(barW, barH));
    bg.setPosition(barX, barY);
    bg.setFillColor(sf::Color(60, 60, 60));
    bg.setOutlineColor(sf::Color(30, 30, 30));
    bg.setOutlineThickness(1.f);
    window.draw(bg);

    // Fill
    float ratio = (maxHp > 0) ? static_cast<float>(hp) / maxHp : 0.f;
    sf::Color fillColor;
    if (ratio > 0.6f) fillColor = sf::Color(30, 200, 30);
    else if (ratio > 0.3f) fillColor = sf::Color(220, 200, 30);
    else fillColor = sf::Color(220, 50, 30);

    sf::RectangleShape fill(sf::Vector2f(barW * ratio, barH));
    fill.setPosition(barX, barY);
    fill.setFillColor(fillColor);
    window.draw(fill);
}

void UIManager::drawPauseMenu(sf::RenderWindow& window) {
    drawBattleOverlay(window);

    if (uiAssetsLoaded) {
        drawPanel(window, panelTexture, 440.f, 235.f, 400.f, 250.f);
    }

    if (!fontLoaded) return;

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(44);
    title.setFillColor(sf::Color::White);
    title.setString("PAUSED");
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition(640.f - tb.width / 2.f, 260.f);
    window.draw(title);

    sf::Text resumeHint;
    resumeHint.setFont(font);
    resumeHint.setCharacterSize(20);
    resumeHint.setFillColor(sf::Color(200, 200, 200));
    resumeHint.setString("Press ESC to Resume");
    sf::FloatRect rb = resumeHint.getLocalBounds();
    resumeHint.setPosition(640.f - rb.width / 2.f, 340.f);
    window.draw(resumeHint);

    sf::Text quitHint;
    quitHint.setFont(font);
    quitHint.setCharacterSize(20);
    quitHint.setFillColor(sf::Color(180, 180, 180));
    quitHint.setString("Press Q to Quit to Menu");
    sf::FloatRect qb = quitHint.getLocalBounds();
    quitHint.setPosition(640.f - qb.width / 2.f, 390.f);
    window.draw(quitHint);
}

void UIManager::drawShopPanel(sf::RenderWindow& window, int playerScore, int selection) {
    drawBattleOverlay(window);

    if (uiAssetsLoaded) {
        drawPanel(window, panelTexture, 340.f, 160.f, 600.f, 400.f);
    }

    if (!fontLoaded) return;

    // Title
    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(36);
    title.setFillColor(sf::Color(255, 215, 0));
    title.setString("Wizard's Shop");
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition(640.f - tb.width / 2.f, 180.f);
    window.draw(title);

    // Score display
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Score: " + std::to_string(playerScore));
    sf::FloatRect sb = scoreText.getLocalBounds();
    scoreText.setPosition(890.f - sb.width, 185.f);
    window.draw(scoreText);

    // Shop items
    struct ShopItem {
        std::string name;
        int cost;
        std::string desc;
    };
    ShopItem items[3] = {
        {"Health Potion", 50, "Restore 30 HP"},
        {"Power Boost", 100, "2x damage next battle"},
        {"Full Heal", 150, "Restore HP to max"}
    };

    for (int i = 0; i < 3; i++) {
        float itemY = 260.f + i * 80.f;
        bool canAfford = playerScore >= items[i].cost;

        if (uiAssetsLoaded) {
            sf::Sprite btn;
            btn.setTexture(i == selection ? buttonTexture : buttonBeige);
            btn.setScale(520.f / 190.f, 60.f / 49.f);
            btn.setPosition(380.f, itemY);
            window.draw(btn);
        }

        sf::Color textColor;
        if (!canAfford) textColor = sf::Color(120, 120, 120);
        else if (i == selection) textColor = sf::Color::Yellow;
        else textColor = uiAssetsLoaded ? sf::Color(50, 50, 80) : sf::Color::White;

        sf::Text itemText;
        itemText.setFont(font);
        itemText.setCharacterSize(22);
        itemText.setFillColor(textColor);
        itemText.setString(items[i].name + "  -  " + std::to_string(items[i].cost) + " pts");
        itemText.setPosition(400.f, itemY + 8.f);
        window.draw(itemText);

        sf::Text descText;
        descText.setFont(font);
        descText.setCharacterSize(14);
        descText.setFillColor(sf::Color(180, 180, 200));
        descText.setString(items[i].desc);
        descText.setPosition(400.f, itemY + 36.f);
        window.draw(descText);
    }

    // Hint
    sf::Text hint;
    hint.setFont(font);
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(120, 120, 120));
    hint.setString("W/S to navigate  |  Enter to buy  |  ESC to close");
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setPosition(640.f - hb.width / 2.f, 530.f);
    window.draw(hint);
}
