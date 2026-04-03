#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

#include "QuestionManager.h"
#include "Player.h"
#include "Enemy.h"
#include "UIManager.h"
#include "SoundManager.h"
#include <SFML/Graphics.hpp>
#include <string>

enum class BattleState {
    ASKING,
    SHOWING_FEEDBACK
};

class BattleSystem {
public:
    BattleSystem(QuestionManager& qm, UIManager& ui, SoundManager& sm);

    void startBattle(Player& player, Enemy& enemy);
    void handleInput(sf::Keyboard::Key key);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    bool isBattleOver() const;
    bool isPlayerDead() const;
    bool isEnemyDead() const;

private:
    void loadNextQuestion();
    std::string getDifficulty(int level);
    std::string getRandomCategory();
    int getDamageForDifficulty(const std::string& diff) const;
    float getComboMultiplier() const;

    QuestionManager& questionManager;
    UIManager& uiManager;
    SoundManager& soundManager;
    Player* currentPlayer = nullptr;
    Enemy* currentEnemy = nullptr;

    Question currentQuestion;
    BattleState battleState = BattleState::ASKING;
    bool battleOver = false;
    bool playerDead = false;
    bool enemyDead = false;

    bool lastAnswerCorrect = false;
    float feedbackTimer = 0.f;
    float feedbackDuration = 1.5f;

    // Combo system
    int comboStreak = 0;
    int lastDamageDealt = 0;

    // Timer system
    float questionTimer = 0.f;
    float questionTimeLimit = 12.f; // seconds to answer
    bool timedOut = false;

    // Turn counter
    int turnCount = 0;
};

#endif
