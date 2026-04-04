#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>

class Player {
public:
    Player();

    bool loadTexture(const std::string& filepath);
    void handleInput();
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    void takeDamage(int amount);
    void gainXP(int amount);
    void checkLevelUp();

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);

    // Stats
    int hp = 100;
    int maxHp = 100;
    int score = 0;
    int xp = 0;
    int level = 1;
    bool powerBoost = false;

private:
    // Spritesheet: 192x320, 32x32 frames, 6 cols x 10 rows
    // Row 0: walk down, Row 2: walk right, Row 4: walk up
    // Left = flipped right
    sf::Texture texture;
    sf::Sprite sprite;
    bool textureLoaded = false;

    // Fallback colored rectangle
    sf::RectangleShape fallbackShape;

    sf::Vector2f velocity;
    float speed = 150.f;

    // Animation
    static const int FRAME_SIZE = 32;
    int animFrame = 0;
    int animRow = 0;       // 0=down, 2=right, 4=up
    float animTimer = 0.f;
    float animSpeed = 0.12f; // seconds per frame
    int numFrames = 6;
    bool facingLeft = false;
    bool moving = false;
};

#endif
