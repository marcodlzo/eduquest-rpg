#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <string>

enum class EnemyType {
    SLIME,
    SKELETON,
    ORC_WARRIOR,
    ORC_SHAMAN
};

class Enemy {
public:
    Enemy(sf::Vector2f waypointA, sf::Vector2f waypointB, EnemyType type = EnemyType::SKELETON);

    void loadTexture(sf::Texture& sharedTexture);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    void takeDamage(int amount);
    bool isDead() const;
    bool isPlayerNearby(sf::Vector2f playerPos) const;

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

    int hp = 60;
    int maxHp = 60;

private:
    sf::Sprite sprite;
    sf::RectangleShape fallbackShape;
    bool textureLoaded = false;

    sf::Vector2f waypointA;
    sf::Vector2f waypointB;

public:
    EnemyType type;

private:
    bool movingToB = true;
    float speed = 60.f;
    float interactRadius = 90.f;

    // Animation
    int frameSize = 32;
    int animFrame = 0;
    int animRow = 0;
    float animTimer = 0.f;
    float animSpeed = 0.15f;
    int numFrames = 6;
    bool facingLeft = false;
};

#endif
