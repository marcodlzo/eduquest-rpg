#include "SoundManager.h"
#include <iostream>

SoundManager::SoundManager() {}

bool SoundManager::init() {
    bool ok = true;
    ok &= loadSound("correct", "assets/sounds/correct.ogg");
    ok &= loadSound("wrong", "assets/sounds/wrong.ogg");
    ok &= loadSound("menu_select", "assets/sounds/menu_select.ogg");
    ok &= loadSound("level_up", "assets/sounds/level_up.ogg");
    ok &= loadSound("battle_start", "assets/sounds/battle_start.ogg");
    ok &= loadSound("enemy_death", "assets/sounds/enemy_death.ogg");
    ok &= loadSound("win", "assets/sounds/win.ogg");
    ok &= loadSound("game_over", "assets/sounds/game_over.ogg");

    if (!ok)
        std::cerr << "Warning: some sounds failed to load" << std::endl;

    // Load background music
    menuMusicLoaded = menuMusic.openFromFile("assets/sounds/bgm_menu.ogg");
    if (menuMusicLoaded) {
        menuMusic.setLoop(true);
        menuMusic.setVolume(40.f);
    } else {
        std::cerr << "Failed to load menu music" << std::endl;
    }

    return ok;
}

bool SoundManager::loadSound(const std::string& name, const std::string& filepath) {
    if (!buffers[name].loadFromFile(filepath)) {
        std::cerr << "Failed to load sound: " << filepath << std::endl;
        return false;
    }
    sounds[name].setBuffer(buffers[name]);
    return true;
}

void SoundManager::playCorrect()     { sounds["correct"].play(); }
void SoundManager::playWrong()       { sounds["wrong"].play(); }
void SoundManager::playMenuSelect()  { sounds["menu_select"].play(); }
void SoundManager::playLevelUp()     { sounds["level_up"].play(); }
void SoundManager::playBattleStart() { sounds["battle_start"].play(); }
void SoundManager::playEnemyDeath()  { sounds["enemy_death"].play(); }
void SoundManager::playWin()         { sounds["win"].play(); }
void SoundManager::playGameOver()    { sounds["game_over"].play(); }

void SoundManager::playMenuMusic() {
    if (!menuMusicLoaded) return;
    gameMusic.stop();
    if (menuMusic.getStatus() != sf::Music::Playing) {
        menuMusic.setLoop(true);
        menuMusic.setVolume(40.f);
        menuMusic.play();
    }
}

void SoundManager::playGameMusic() {
    menuMusic.stop();
    // Game music will be added later
}

void SoundManager::stopMusic() {
    menuMusic.stop();
    gameMusic.stop();
}
