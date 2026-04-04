#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Enemy.h"
#include "QuestionManager.h"
#include "BattleSystem.h"
#include "UIManager.h"
#include "SoundManager.h"

// Forward declare GameState (defined in Game.h)
enum class GameState;

struct Chest {
    sf::Vector2f position;
    bool opened = false;
    enum class Type { HEALTH, SCORE } type;
    int value; // HP restored or score bonus
};

struct ShopNPC {
    sf::Vector2f position;
    sf::Sprite sprite;
    int animFrame = 0;
    float animTimer = 0.f;
    float animSpeed = 0.2f;
    int numFrames = 4;
    int frameSize = 32;
};

class GameManager {
public:
    GameManager();

    bool init();
    void handleInput(sf::Event& event, GameState& state);
    void update(float deltaTime, GameState& state);
    void draw(sf::RenderWindow& window, GameState state);

    Player& getPlayer();
    UIManager& getUIManager();
    SoundManager& getSoundManager();
    void reset();

    int getCurrentWave() const { return currentWave; }
    int getMaxWaves() const { return maxWaves; }

private:
    void initTilemap();
    void initEnemies();
    void initChests();
    void drawTilemap(sf::RenderWindow& window);
    void drawWaveTransition(sf::RenderWindow& window);
    void drawChests(sf::RenderWindow& window);
    void drawChestPopup(sf::RenderWindow& window);
    bool isWalkable(float x, float y);
    int getNearbyEnemyIndex();
    int getNearbyChestIndex();
    void drawInteractPrompt(sf::RenderWindow& window);
    void drawSlashAnimation(sf::RenderWindow& window);
    void drawShopNPC(sf::RenderWindow& window);
    bool isNearShopNPC();

    static const int MAP_WIDTH = 30;
    static const int MAP_HEIGHT = 20;
    static const int TILE_SIZE = 48;
    int tilemap[MAP_HEIGHT][MAP_WIDTH];

    // Tile textures
    sf::Texture grassTexture;
    sf::Texture treeTexture;
    sf::Sprite grassSprite;
    sf::Sprite treeSprite;
    bool tilesLoaded = false;

    // New tile textures
    sf::Texture waterTexture;
    sf::Texture pathTexture;
    sf::Texture fenceTexture;
    sf::Texture houseTexture;
    sf::Sprite waterSprite;
    sf::Sprite pathSprite;
    sf::Sprite fenceSprite;
    sf::Sprite houseSprite;

    // Enemy textures
    sf::Texture skeletonTexture;
    sf::Texture slimeTexture;
    sf::Texture orcWarriorTexture;
    sf::Texture orcShamanTexture;

    // Shop NPC
    sf::Texture wizardTexture;
    ShopNPC shopNPC;
    bool wizardLoaded = false;
    bool nearShopNPC = false;
    int shopSelection = 0;

    // Chest texture
    sf::Texture chestTexture;
    sf::Sprite chestSprite;
    bool chestTextureLoaded = false;

    // Slash animation
    sf::Texture slashTexture;
    sf::Sprite slashSprite;
    bool slashLoaded = false;
    bool playingSlash = false;
    int slashFrame = 0;
    float slashTimer = 0.f;
    float slashSpeed = 0.06f;
    int slashTotalFrames = 8;
    sf::Vector2f slashPosition;

    Player player;
    std::vector<Enemy> enemies;
    std::vector<Chest> chests;
    QuestionManager questionManager;
    UIManager uiManager;
    SoundManager soundManager;
    BattleSystem battleSystem;

    int currentEnemyIndex = -1;
    int nearbyEnemyIndex = -1;
    int nearbyChestIndex = -1;

    // Chest popup
    bool showingChestPopup = false;
    float chestPopupTimer = 0.f;
    float chestPopupDuration = 2.f;
    std::string chestPopupText;
    sf::Color chestPopupColor;

    // Wave system
    int currentWave = 1;
    static const int maxWaves = 3;
    bool waveTransition = false;
    float waveTransitionTimer = 0.f;
    float waveTransitionDuration = 3.f;

    // Background tint per wave
    sf::Color getWaveTint() const;
};

#endif
