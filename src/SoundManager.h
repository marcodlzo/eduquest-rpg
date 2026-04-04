#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <string>
#include <map>

class SoundManager {
public:
    SoundManager();

    bool init();

    void playCorrect();
    void playWrong();
    void playMenuSelect();
    void playLevelUp();
    void playBattleStart();
    void playEnemyDeath();
    void playWin();
    void playGameOver();

    void playMenuMusic();
    void playGameMusic();
    void stopMusic();

private:
    sf::Music menuMusic;
    sf::Music gameMusic;
    bool menuMusicLoaded = false;
    bool gameMusicLoaded = false;
    bool loadSound(const std::string& name, const std::string& filepath);

    std::map<std::string, sf::SoundBuffer> buffers;
    std::map<std::string, sf::Sound> sounds;
};

#endif
