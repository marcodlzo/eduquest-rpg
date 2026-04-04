#include "Enemy.h"
#include <cmath>

Enemy::Enemy(sf::Vector2f waypointA, sf::Vector2f waypointB, EnemyType type)
    : waypointA(waypointA), waypointB(waypointB), type(type)
{
    // Slimes are weaker, skeletons are tougher
    if (type == EnemyType::SLIME) {
        hp = 50;
        maxHp = 50;
        speed = 40.f;
    } else if (type == EnemyType::SKELETON) {
        hp = 75;
        maxHp = 75;
        speed = 60.f;
    } else if (type == EnemyType::ORC_WARRIOR) {
        hp = 90;
        maxHp = 90;
        speed = 50.f;
    } else if (type == EnemyType::ORC_SHAMAN) {
        hp = 60;
        maxHp = 60;
        speed = 70.f;
    }

    fallbackShape.setSize(sf::Vector2f(48.f, 48.f));
    sf::Color fallbackColor;
    if (type == EnemyType::SLIME) fallbackColor = sf::Color(50, 200, 50);
    else if (type == EnemyType::SKELETON) fallbackColor = sf::Color(220, 50, 50);
    else if (type == EnemyType::ORC_WARRIOR) fallbackColor = sf::Color(100, 150, 50);
    else fallbackColor = sf::Color(150, 50, 200);
    fallbackShape.setFillColor(fallbackColor);
    fallbackShape.setPosition(waypointA);

    sprite.setPosition(waypointA);
}

void Enemy::loadTexture(sf::Texture& sharedTexture) {
    sprite.setTexture(sharedTexture);

    if (type == EnemyType::SLIME) {
        // Slime spritesheet: 512x192, 64x64 frames
        // Row 0: 4 frames idle
        frameSize = 64;
        numFrames = 4;
        animRow = 0;
        sprite.setTextureRect(sf::IntRect(0, 0, frameSize, frameSize));
        sprite.setScale(48.f / 64.f, 48.f / 64.f); // 64 -> 48px
    } else if (type == EnemyType::SKELETON) {
        frameSize = 32;
        numFrames = 6;
        sprite.setTextureRect(sf::IntRect(0, 0, frameSize, frameSize));
        sprite.setScale(1.5f, 1.5f);
    } else {
        // Orc Warrior / Orc Shaman: 128x32 horizontal strip, 4 frames of 32x32
        frameSize = 32;
        numFrames = 4;
        animRow = 0;
        sprite.setTextureRect(sf::IntRect(0, 0, frameSize, frameSize));
        sprite.setScale(1.5f, 1.5f);
    }

    textureLoaded = true;
}

void Enemy::update(float deltaTime) {
    sf::Vector2f target = movingToB ? waypointB : waypointA;
    sf::Vector2f pos = textureLoaded ? sprite.getPosition() : fallbackShape.getPosition();
    sf::Vector2f direction = target - pos;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance < 2.f) {
        movingToB = !movingToB;
        return;
    }

    sf::Vector2f normalized = direction / distance;
    sf::Vector2f movement = normalized * speed * deltaTime;

    // Determine animation row based on direction
    if (type == EnemyType::SKELETON) {
        if (std::abs(direction.x) > std::abs(direction.y)) {
            animRow = 2; // side
            facingLeft = (direction.x < 0);
        } else {
            animRow = (direction.y > 0) ? 0 : 4;
            facingLeft = false;
        }
    } else {
        // Slime: just use row 0 idle animation
        animRow = 0;
        facingLeft = (direction.x < 0);
    }

    // Animate
    animTimer += deltaTime;
    if (animTimer >= animSpeed) {
        animTimer = 0.f;
        animFrame = (animFrame + 1) % numFrames;
    }

    if (textureLoaded) {
        sprite.move(movement);
        sprite.setTextureRect(sf::IntRect(
            animFrame * frameSize,
            animRow * frameSize,
            frameSize, frameSize));

        float sc = 48.f / frameSize; // 1.5 for 32px, 0.75 for 64px
        if (facingLeft) {
            sprite.setScale(-sc, sc);
            sprite.setOrigin(frameSize, 0);
        } else {
            sprite.setScale(sc, sc);
            sprite.setOrigin(0, 0);
        }
    } else {
        fallbackShape.move(movement);
    }
}

void Enemy::draw(sf::RenderWindow& window) {
    if (textureLoaded) {
        window.draw(sprite);
    } else {
        window.draw(fallbackShape);
    }
}

void Enemy::takeDamage(int amount) {
    hp -= amount;
    if (hp < 0) hp = 0;
}

bool Enemy::isDead() const {
    return hp <= 0;
}

bool Enemy::isPlayerNearby(sf::Vector2f playerPos) const {
    sf::Vector2f pos = textureLoaded ? sprite.getPosition() : fallbackShape.getPosition();
    if (textureLoaded && facingLeft) pos.x -= 48.f;
    sf::Vector2f center(pos.x + 24.f, pos.y + 24.f);
    sf::Vector2f playerCenter(playerPos.x + 24.f, playerPos.y + 24.f);
    sf::Vector2f diff = center - playerCenter;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    return dist < interactRadius;
}

sf::FloatRect Enemy::getBounds() const {
    if (textureLoaded) {
        sf::Vector2f pos = sprite.getPosition();
        if (facingLeft) return sf::FloatRect(pos.x - 48.f, pos.y, 48.f, 48.f);
        return sf::FloatRect(pos.x, pos.y, 48.f, 48.f);
    }
    return fallbackShape.getGlobalBounds();
}

sf::Vector2f Enemy::getPosition() const {
    if (textureLoaded) {
        sf::Vector2f pos = sprite.getPosition();
        if (facingLeft) return sf::Vector2f(pos.x - 48.f, pos.y);
        return pos;
    }
    return fallbackShape.getPosition();
}
