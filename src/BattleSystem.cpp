#include "BattleSystem.h"
#include <random>
#include <chrono>
#include <cmath>

BattleSystem::BattleSystem(QuestionManager& qm, UIManager& ui, SoundManager& sm)
    : questionManager(qm), uiManager(ui), soundManager(sm) {}

void BattleSystem::startBattle(Player& player, Enemy& enemy) {
    currentPlayer = &player;
    currentEnemy = &enemy;
    battleOver = false;
    playerDead = false;
    enemyDead = false;
    comboStreak = 0;
    turnCount = 0;
    lastDamageDealt = 0;
    battleState = BattleState::ASKING;
    loadNextQuestion();
}

void BattleSystem::loadNextQuestion() {
    std::string difficulty = getDifficulty(currentPlayer->level);
    std::string category = getRandomCategory();
    currentQuestion = questionManager.getRandomQuestion(category, difficulty);
    battleState = BattleState::ASKING;
    questionTimer = 0.f;
    timedOut = false;
    turnCount++;
}

std::string BattleSystem::getDifficulty(int level) {
    if (level <= 2) return "Easy";
    if (level <= 4) return "Medium";
    return "Hard";
}

std::string BattleSystem::getRandomCategory() {
    static auto rng = std::mt19937(
        std::chrono::steady_clock::now().time_since_epoch().count());
    std::string categories[] = {"Programming", "Science", "Computer Science"};
    std::uniform_int_distribution<int> dist(0, 2);
    return categories[dist(rng)];
}

int BattleSystem::getDamageForDifficulty(const std::string& diff) const {
    if (diff == "Easy") return 15;
    if (diff == "Medium") return 25;
    return 40; // Hard
}

float BattleSystem::getComboMultiplier() const {
    if (comboStreak <= 1) return 1.0f;
    if (comboStreak == 2) return 1.5f;
    if (comboStreak == 3) return 2.0f;
    return 2.5f; // 4+
}

void BattleSystem::handleInput(sf::Keyboard::Key key) {
    if (battleState != BattleState::ASKING) return;

    char answer = '\0';
    if (key == sf::Keyboard::A) answer = 'A';
    else if (key == sf::Keyboard::B) answer = 'B';
    else if (key == sf::Keyboard::C) answer = 'C';
    else if (key == sf::Keyboard::D) answer = 'D';
    else return;

    if (answer == currentQuestion.correctAnswer) {
        lastAnswerCorrect = true;
        comboStreak++;

        // Base damage from difficulty + combo multiplier
        int baseDmg = getDamageForDifficulty(currentQuestion.difficulty);
        if (currentPlayer->powerBoost)
            baseDmg *= 2;
        float mult = getComboMultiplier();

        // Speed bonus: answer faster = bonus damage (up to +10)
        float timeRatio = 1.f - (questionTimer / questionTimeLimit);
        int speedBonus = (int)(timeRatio * 10.f);

        lastDamageDealt = (int)(baseDmg * mult) + speedBonus;
        currentEnemy->takeDamage(lastDamageDealt);
        currentPlayer->score += lastDamageDealt;
        soundManager.playCorrect();
    } else {
        lastAnswerCorrect = false;
        comboStreak = 0;
        lastDamageDealt = 0;
        currentPlayer->takeDamage(10);
        soundManager.playWrong();
    }

    feedbackTimer = 0.f;
    battleState = BattleState::SHOWING_FEEDBACK;
}

void BattleSystem::update(float deltaTime) {
    if (battleState == BattleState::ASKING) {
        // Update question timer
        questionTimer += deltaTime;
        if (questionTimer >= questionTimeLimit) {
            // Time's up — treat as wrong answer
            timedOut = true;
            lastAnswerCorrect = false;
            comboStreak = 0;
            lastDamageDealt = 0;
            currentPlayer->takeDamage(10);
            soundManager.playWrong();
            feedbackTimer = 0.f;
            battleState = BattleState::SHOWING_FEEDBACK;
        }
    } else if (battleState == BattleState::SHOWING_FEEDBACK) {
        feedbackTimer += deltaTime;
        if (feedbackTimer >= feedbackDuration) {
            if (currentEnemy->isDead()) {
                enemyDead = true;
                battleOver = true;
                currentPlayer->powerBoost = false;
                int prevLevel = currentPlayer->level;
                currentPlayer->gainXP(20);
                if (currentPlayer->level > prevLevel)
                    soundManager.playLevelUp();
                soundManager.playEnemyDeath();
            } else if (currentPlayer->hp <= 0) {
                playerDead = true;
                battleOver = true;
                currentPlayer->powerBoost = false;
            } else {
                loadNextQuestion();
            }
        }
    }
}

void BattleSystem::draw(sf::RenderWindow& window) {
    uiManager.drawBattleOverlay(window);

    if (battleState == BattleState::ASKING) {
        float timeRatio = questionTimer / questionTimeLimit;
        uiManager.drawQuestionPanel(window, currentQuestion, comboStreak,
                                     getComboMultiplier(), timeRatio, turnCount);
    } else if (battleState == BattleState::SHOWING_FEEDBACK) {
        uiManager.drawFeedback(window, lastAnswerCorrect,
                                currentQuestion.correctAnswer,
                                lastDamageDealt, comboStreak, timedOut);
    }
}

bool BattleSystem::isBattleOver() const { return battleOver; }
bool BattleSystem::isPlayerDead() const { return playerDead; }
bool BattleSystem::isEnemyDead() const { return enemyDead; }
