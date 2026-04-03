#include "Player.h"
#include <iostream>

Player::Player() : velocity(0.f, 0.f) {
    fallbackShape.setSize(sf::Vector2f(48.f, 48.f));
    fallbackShape.setFillColor(sf::Color(50, 120, 220));
    fallbackShape.setPosition(2 * 48.f, 2 * 48.f);

    sprite.setScale(1.5f, 1.5f); // 32x32 -> 48x48
}

bool Player::loadTexture(const std::string& filepath) {
    if (texture.loadFromFile(filepath)) {
        textureLoaded = true;
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, FRAME_SIZE, FRAME_SIZE));
        sprite.setScale(1.5f, 1.5f);
        sprite.setPosition(fallbackShape.getPosition());
        return true;
    }
    std::cerr << "Failed to load player texture: " << filepath << std::endl;
    return false;
}

void Player::handleInput() {
    velocity = sf::Vector2f(0.f, 0.f);
    moving = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        velocity.y = -speed;
        animRow = 4; // up
        moving = true;
        facingLeft = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        velocity.y = speed;
        animRow = 0; // down
        moving = true;
        facingLeft = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x = -speed;
        animRow = 2; // right row, but flipped
        moving = true;
        facingLeft = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocity.x = speed;
        animRow = 2; // right
        moving = true;
        facingLeft = false;
    }
}

void Player::update(float deltaTime) {
    // Move
    if (textureLoaded) {
        sprite.move(velocity * deltaTime);
    } else {
        fallbackShape.move(velocity * deltaTime);
    }

    // Animate
    if (moving) {
        animTimer += deltaTime;
        if (animTimer >= animSpeed) {
            animTimer = 0.f;
            animFrame = (animFrame + 1) % numFrames;
        }
    } else {
        animFrame = 0;
        animTimer = 0.f;
    }

    if (textureLoaded) {
        sprite.setTextureRect(sf::IntRect(
            animFrame * FRAME_SIZE,
            animRow * FRAME_SIZE,
            FRAME_SIZE, FRAME_SIZE));

        // Flip for left movement
        if (facingLeft) {
            sprite.setScale(-1.5f, 1.5f);
            sprite.setOrigin(FRAME_SIZE, 0);
        } else {
            sprite.setScale(1.5f, 1.5f);
            sprite.setOrigin(0, 0);
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    if (textureLoaded) {
        window.draw(sprite);
    } else {
        window.draw(fallbackShape);
    }
}

void Player::takeDamage(int amount) {
    hp -= amount;
    if (hp < 0) hp = 0;
}

void Player::gainXP(int amount) {
    xp += amount;
    checkLevelUp();
}

void Player::checkLevelUp() {
    int threshold = level * 50;
    while (xp >= threshold) {
        xp -= threshold;
        level++;
        maxHp += 10;
        hp = maxHp;
        threshold = level * 50;
    }
}

sf::FloatRect Player::getBounds() const {
    if (textureLoaded) {
        // Return a 48x48 bounds at the sprite position
        sf::Vector2f pos = sprite.getPosition();
        if (facingLeft) {
            return sf::FloatRect(pos.x - 48.f, pos.y, 48.f, 48.f);
        }
        return sf::FloatRect(pos.x, pos.y, 48.f, 48.f);
    }
    return fallbackShape.getGlobalBounds();
}

sf::Vector2f Player::getPosition() const {
    if (textureLoaded) {
        sf::Vector2f pos = sprite.getPosition();
        if (facingLeft) return sf::Vector2f(pos.x - 48.f, pos.y);
        return pos;
    }
    return fallbackShape.getPosition();
}

void Player::setPosition(float x, float y) {
    if (textureLoaded) {
        if (facingLeft) {
            sprite.setPosition(x + 48.f, y);
        } else {
            sprite.setPosition(x, y);
        }
    }
    fallbackShape.setPosition(x, y);
}
