#include "Game.h"
#include <iostream>

Game::Game()
    : window(sf::VideoMode(1280, 720), "EduQuest RPG")
    , state(GameState::MAIN_MENU)
{
    window.setFramerateLimit(60);

    if (!gameManager.init()) {
        std::cerr << "Failed to initialize GameManager!" << std::endl;
    }
}

void Game::run() {
    gameManager.getSoundManager().playMenuMusic();

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        draw();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return;
        }

        if (event.type == sf::Event::KeyPressed) {
            if (state == GameState::MAIN_MENU) {
                UIManager& ui = gameManager.getUIManager();
                if (ui.showInstructions) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        ui.showInstructions = false;
                    }
                } else {
                    if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
                        ui.menuSelection = (ui.menuSelection + 2) % 3;
                        gameManager.getSoundManager().playMenuSelect();
                    } else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
                        ui.menuSelection = (ui.menuSelection + 1) % 3;
                        gameManager.getSoundManager().playMenuSelect();
                    } else if (event.key.code == sf::Keyboard::Return) {
                        if (ui.menuSelection == 0) {
                            gameManager.reset();
                            gameManager.getSoundManager().playGameMusic();
                            state = GameState::PLAYING;
                        } else if (ui.menuSelection == 1) {
                            ui.showInstructions = true;
                        } else if (ui.menuSelection == 2) {
                            window.close();
                        }
                    }
                }
            } else if (state == GameState::PAUSED) {
                if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Return) {
                    gameManager.getSoundManager().playGameMusic();
                    state = GameState::PLAYING;
                } else if (event.key.code == sf::Keyboard::Q) {
                    gameManager.getSoundManager().playMenuMusic();
                    state = GameState::MAIN_MENU;
                }
            } else if (state == GameState::PLAYING || state == GameState::BATTLE || state == GameState::SHOPPING) {
                if (state == GameState::PLAYING && event.key.code == sf::Keyboard::Escape) {
                    state = GameState::PAUSED;
                } else {
                    gameManager.handleInput(event, state);
                }
            } else if (state == GameState::GAME_OVER || state == GameState::WIN) {
                if (event.key.code == sf::Keyboard::Return) {
                    gameManager.getSoundManager().playMenuMusic();
                    state = GameState::MAIN_MENU;
                }
            }
        }
    }
}

void Game::update(float deltaTime) {
    if (state == GameState::PLAYING || state == GameState::BATTLE || state == GameState::SHOPPING) {
        gameManager.update(deltaTime, state);
    }
}

void Game::draw() {
    window.clear();

    if (state == GameState::MAIN_MENU) {
        UIManager& ui = gameManager.getUIManager();
        if (ui.showInstructions)
            ui.drawInstructions(window);
        else
            ui.drawMainMenu(window);
    } else if (state == GameState::PLAYING || state == GameState::BATTLE || state == GameState::SHOPPING) {
        gameManager.draw(window, state);
    } else if (state == GameState::PAUSED) {
        gameManager.draw(window, GameState::PLAYING); // Draw frozen game world
        gameManager.getUIManager().drawPauseMenu(window);
    } else if (state == GameState::GAME_OVER) {
        gameManager.getUIManager().drawGameOver(window, gameManager.getPlayer().score);
    } else if (state == GameState::WIN) {
        Player& p = gameManager.getPlayer();
        gameManager.getUIManager().drawWin(window, p.score, p.level);
    }

    window.display();
}
