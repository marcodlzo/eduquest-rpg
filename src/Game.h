#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "GameManager.h"

// Game states
enum class GameState {
    MAIN_MENU,
    PLAYING,
    BATTLE,
    SHOPPING,
    PAUSED,
    GAME_OVER,
    WIN
};

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void draw();

    sf::RenderWindow window;
    sf::Clock clock;
    GameState state;
    GameManager gameManager;
};

#endif
