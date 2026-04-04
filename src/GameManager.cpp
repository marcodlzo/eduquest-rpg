#include "GameManager.h"
#include "Game.h"
#include <iostream>
#include <cmath>

GameManager::GameManager()
    : battleSystem(questionManager, uiManager, soundManager) {}

bool GameManager::init() {
    uiManager.loadFont("assets/fonts/arial.ttf");
    uiManager.loadUIAssets();
    soundManager.init();

    if (!questionManager.loadQuestions("data/questions.json")) {
        std::cerr << "Failed to load questions!" << std::endl;
        return false;
    }

    // Load tile textures
    if (grassTexture.loadFromFile("assets/images/grass.png") &&
        treeTexture.loadFromFile("assets/images/tree.png")) {
        grassTexture.setRepeated(false);
        grassSprite.setTexture(grassTexture);
        grassSprite.setScale(48.f / 16.f, 48.f / 16.f);

        treeSprite.setTexture(treeTexture);
        treeSprite.setScale(48.f / 64.f, 48.f / 80.f);

        tilesLoaded = true;
    }

    // Load new tile textures
    if (waterTexture.loadFromFile("assets/images/water.png")) {
        waterSprite.setTexture(waterTexture);
        waterSprite.setScale(48.f / 16.f, 48.f / 16.f);
    }
    if (pathTexture.loadFromFile("assets/images/path_tile.png")) {
        pathSprite.setTexture(pathTexture);
        pathSprite.setScale(48.f / 16.f, 48.f / 16.f);
    }
    if (fenceTexture.loadFromFile("assets/images/fence.png")) {
        fenceSprite.setTexture(fenceTexture);
        fenceSprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
        fenceSprite.setScale(48.f / 16.f, 48.f / 16.f);
    }
    if (houseTexture.loadFromFile("assets/images/house.png")) {
        houseSprite.setTexture(houseTexture);
        // 96x128 → 2x3 tiles (96px wide, 144px tall)
        houseSprite.setScale(96.f / 96.f, 144.f / 128.f);
    }

    // Load player texture
    player.loadTexture("assets/images/player.png");

    // Load enemy textures
    skeletonTexture.loadFromFile("assets/images/skeleton.png");
    slimeTexture.loadFromFile("assets/images/slime.png");
    orcWarriorTexture.loadFromFile("assets/images/orc_warrior.png");
    orcShamanTexture.loadFromFile("assets/images/orc_shaman.png");

    // Load wizard NPC texture (128x32, 4 frames of 32x32)
    if (wizardTexture.loadFromFile("assets/images/wizard_npc.png")) {
        shopNPC.sprite.setTexture(wizardTexture);
        shopNPC.sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
        shopNPC.sprite.setScale(1.5f, 1.5f);
        shopNPC.position = sf::Vector2f(4 * TILE_SIZE, 3 * TILE_SIZE);
        shopNPC.sprite.setPosition(shopNPC.position);
        wizardLoaded = true;
    }

    // Load chest texture (16x16, scale to 48x48)
    if (chestTexture.loadFromFile("assets/images/chest.png")) {
        chestSprite.setTexture(chestTexture);
        chestSprite.setScale(48.f / 16.f, 48.f / 16.f);
        chestTextureLoaded = true;
    }

    // Load slash animation (512x64, 8 frames of 64x64)
    if (slashTexture.loadFromFile("assets/images/slash.png")) {
        slashSprite.setTexture(slashTexture);
        slashSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
        slashSprite.setScale(1.5f, 1.5f);
        slashLoaded = true;
    }

    initTilemap();
    initEnemies();
    initChests();
    player.setPosition(2 * TILE_SIZE, 5 * TILE_SIZE);

    return true;
}

void GameManager::initTilemap() {
    // Start with all grass
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            tilemap[y][x] = 0;
        }
    }

    // Border walls (trees)
    for (int x = 0; x < MAP_WIDTH; x++) {
        tilemap[0][x] = 1;
        tilemap[MAP_HEIGHT - 1][x] = 1;
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        tilemap[y][0] = 1;
        tilemap[y][MAP_WIDTH - 1] = 1;
    }

    // Interior tree clusters
    for (int x = 5; x <= 10; x++) tilemap[6][x] = 1;
    for (int y = 3; y <= 8; y++) tilemap[y][15] = 1;
    for (int x = 18; x <= 24; x++) tilemap[13][x] = 1;
    tilemap[10][8] = 1; tilemap[10][9] = 1;
    tilemap[11][8] = 1; tilemap[11][9] = 1;
    for (int y = 10; y <= 15; y++) tilemap[y][22] = 1;

    // Water pond (upper-right area)
    for (int x = 24; x <= 27; x++) {
        for (int y = 1; y <= 3; y++) {
            tilemap[y][x] = 2;
        }
    }

    // Paths connecting areas
    for (int x = 2; x <= 12; x++) tilemap[5][x] = 3;  // horizontal path
    for (int y = 5; y <= 9; y++) tilemap[y][12] = 3;   // vertical connector
    for (int x = 12; x <= 20; x++) tilemap[9][x] = 3;  // mid horizontal
    for (int y = 5; y <= 17; y++) tilemap[y][2] = 3;    // left vertical path

    // Fences around shop/house area
    tilemap[1][4] = 4;
    tilemap[2][4] = 4;
    tilemap[3][4] = 4;
    tilemap[4][1] = 4; tilemap[4][2] = 4; tilemap[4][3] = 4; tilemap[4][4] = 4;

    // House at (1,1) — marks 2x3 tiles as type 5
    tilemap[1][1] = 5; tilemap[1][2] = 5;
    tilemap[2][1] = 5; tilemap[2][2] = 5;
    tilemap[3][1] = 5; tilemap[3][2] = 5;
}

void GameManager::initEnemies() {
    enemies.clear();

    if (currentWave == 1) {
        enemies.reserve(3);
        enemies.emplace_back(sf::Vector2f(20 * 48.f, 3 * 48.f),
                             sf::Vector2f(26 * 48.f, 3 * 48.f), EnemyType::SLIME);
        enemies.emplace_back(sf::Vector2f(4 * 48.f, 8 * 48.f),
                             sf::Vector2f(4 * 48.f, 14 * 48.f), EnemyType::SLIME);
        enemies.emplace_back(sf::Vector2f(11 * 48.f, 10 * 48.f),
                             sf::Vector2f(18 * 48.f, 10 * 48.f), EnemyType::SLIME);
    } else if (currentWave == 2) {
        enemies.reserve(4);
        enemies.emplace_back(sf::Vector2f(6 * 48.f, 3 * 48.f),
                             sf::Vector2f(12 * 48.f, 3 * 48.f), EnemyType::SLIME);
        enemies.emplace_back(sf::Vector2f(20 * 48.f, 8 * 48.f),
                             sf::Vector2f(26 * 48.f, 8 * 48.f), EnemyType::SLIME);
        enemies.emplace_back(sf::Vector2f(25 * 48.f, 10 * 48.f),
                             sf::Vector2f(25 * 48.f, 16 * 48.f), EnemyType::SKELETON);
        enemies.emplace_back(sf::Vector2f(4 * 48.f, 14 * 48.f),
                             sf::Vector2f(12 * 48.f, 14 * 48.f), EnemyType::ORC_WARRIOR);
    } else if (currentWave == 3) {
        enemies.reserve(4);
        enemies.emplace_back(sf::Vector2f(6 * 48.f, 3 * 48.f),
                             sf::Vector2f(14 * 48.f, 3 * 48.f), EnemyType::SKELETON);
        enemies.emplace_back(sf::Vector2f(20 * 48.f, 5 * 48.f),
                             sf::Vector2f(27 * 48.f, 5 * 48.f), EnemyType::SKELETON);
        enemies.emplace_back(sf::Vector2f(4 * 48.f, 12 * 48.f),
                             sf::Vector2f(12 * 48.f, 12 * 48.f), EnemyType::ORC_WARRIOR);
        enemies.emplace_back(sf::Vector2f(18 * 48.f, 16 * 48.f),
                             sf::Vector2f(26 * 48.f, 16 * 48.f), EnemyType::ORC_SHAMAN);

        for (auto& enemy : enemies) {
            if (enemy.type == EnemyType::SKELETON) {
                enemy.hp = 100;
                enemy.maxHp = 100;
            }
        }
    }

    for (auto& enemy : enemies) {
        if (enemy.type == EnemyType::SLIME)
            enemy.loadTexture(slimeTexture);
        else if (enemy.type == EnemyType::SKELETON)
            enemy.loadTexture(skeletonTexture);
        else if (enemy.type == EnemyType::ORC_WARRIOR)
            enemy.loadTexture(orcWarriorTexture);
        else if (enemy.type == EnemyType::ORC_SHAMAN)
            enemy.loadTexture(orcShamanTexture);
    }
}

void GameManager::initChests() {
    chests.clear();

    // Place chests in interesting spots — different per wave
    if (currentWave == 1) {
        chests.push_back({sf::Vector2f(13 * 48.f, 3 * 48.f), false, Chest::Type::HEALTH, 30});
        chests.push_back({sf::Vector2f(3 * 48.f, 16 * 48.f), false, Chest::Type::SCORE, 50});
    } else if (currentWave == 2) {
        chests.push_back({sf::Vector2f(14 * 48.f, 10 * 48.f), false, Chest::Type::HEALTH, 30});
        chests.push_back({sf::Vector2f(27 * 48.f, 3 * 48.f), false, Chest::Type::HEALTH, 30});
        chests.push_back({sf::Vector2f(8 * 48.f, 17 * 48.f), false, Chest::Type::SCORE, 75});
    } else if (currentWave == 3) {
        chests.push_back({sf::Vector2f(2 * 48.f, 8 * 48.f), false, Chest::Type::HEALTH, 50});
        chests.push_back({sf::Vector2f(27 * 48.f, 17 * 48.f), false, Chest::Type::HEALTH, 50});
        chests.push_back({sf::Vector2f(14 * 48.f, 7 * 48.f), false, Chest::Type::SCORE, 100});
    }
}

int GameManager::getNearbyChestIndex() {
    sf::Vector2f ppos = player.getPosition();
    sf::Vector2f pcenter(ppos.x + 24.f, ppos.y + 24.f);

    for (int i = 0; i < (int)chests.size(); i++) {
        if (chests[i].opened) continue;
        sf::Vector2f ccenter(chests[i].position.x + 24.f, chests[i].position.y + 24.f);
        sf::Vector2f diff = ccenter - pcenter;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < 80.f)
            return i;
    }
    return -1;
}

bool GameManager::isNearShopNPC() {
    sf::Vector2f ppos = player.getPosition();
    sf::Vector2f pcenter(ppos.x + 24.f, ppos.y + 24.f);
    sf::Vector2f ncenter(shopNPC.position.x + 24.f, shopNPC.position.y + 24.f);
    sf::Vector2f diff = ncenter - pcenter;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    return dist < 80.f;
}

void GameManager::drawShopNPC(sf::RenderWindow& window) {
    if (!wizardLoaded) return;
    shopNPC.sprite.setPosition(shopNPC.position);
    window.draw(shopNPC.sprite);
}

bool GameManager::isWalkable(float x, float y) {
    int corners[4][2] = {
        {(int)(x / TILE_SIZE), (int)(y / TILE_SIZE)},
        {(int)((x + 47) / TILE_SIZE), (int)(y / TILE_SIZE)},
        {(int)(x / TILE_SIZE), (int)((y + 47) / TILE_SIZE)},
        {(int)((x + 47) / TILE_SIZE), (int)((y + 47) / TILE_SIZE)}
    };

    for (auto& c : corners) {
        int tx = c[0], ty = c[1];
        if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT)
            return false;
        int tile = tilemap[ty][tx];
        if (tile == 1 || tile == 2 || tile == 4 || tile == 5)
            return false;
    }
    return true;
}

int GameManager::getNearbyEnemyIndex() {
    for (int i = 0; i < (int)enemies.size(); i++) {
        if (enemies[i].isDead()) continue;
        if (enemies[i].isPlayerNearby(player.getPosition()))
            return i;
    }
    return -1;
}

void GameManager::handleInput(sf::Event& event, GameState& state) {
    if (state == GameState::BATTLE) {
        if (event.type == sf::Event::KeyPressed) {
            battleSystem.handleInput(event.key.code);
        }
    } else if (state == GameState::PLAYING) {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
            if (waveTransition) return;

            // Priority: enemy battle > chest interaction
            if (nearbyEnemyIndex >= 0) {
                currentEnemyIndex = nearbyEnemyIndex;
                battleSystem.startBattle(player, enemies[currentEnemyIndex]);
                soundManager.playBattleStart();
                state = GameState::BATTLE;
            } else if (nearbyChestIndex >= 0) {
                Chest& chest = chests[nearbyChestIndex];
                chest.opened = true;
                soundManager.playCorrect(); // Reuse correct sound for chest open

                if (chest.type == Chest::Type::HEALTH) {
                    player.hp += chest.value;
                    if (player.hp > player.maxHp) player.hp = player.maxHp;
                    chestPopupText = "+" + std::to_string(chest.value) + " HP!";
                    chestPopupColor = sf::Color::Green;
                } else {
                    player.score += chest.value;
                    chestPopupText = "+" + std::to_string(chest.value) + " Score!";
                    chestPopupColor = sf::Color(255, 215, 0);
                }

                showingChestPopup = true;
                chestPopupTimer = 0.f;
            } else if (nearShopNPC) {
                shopSelection = 0;
                state = GameState::SHOPPING;
            }
        }
    } else if (state == GameState::SHOPPING) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                state = GameState::PLAYING;
            } else if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
                shopSelection = (shopSelection + 2) % 3;
                soundManager.playMenuSelect();
            } else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
                shopSelection = (shopSelection + 1) % 3;
                soundManager.playMenuSelect();
            } else if (event.key.code == sf::Keyboard::Return) {
                int costs[] = {50, 100, 150};
                if (player.score >= costs[shopSelection]) {
                    player.score -= costs[shopSelection];
                    if (shopSelection == 0) {
                        // Health Potion: +30 HP
                        player.hp += 30;
                        if (player.hp > player.maxHp) player.hp = player.maxHp;
                    } else if (shopSelection == 1) {
                        // Power Boost
                        player.powerBoost = true;
                    } else if (shopSelection == 2) {
                        // Full Heal
                        player.hp = player.maxHp;
                    }
                    soundManager.playCorrect();
                } else {
                    soundManager.playWrong();
                }
            }
        }
    }
}

void GameManager::update(float deltaTime, GameState& state) {
    if (state == GameState::PLAYING) {
        // Handle wave transition timer
        if (waveTransition) {
            waveTransitionTimer += deltaTime;
            if (waveTransitionTimer >= waveTransitionDuration) {
                waveTransition = false;
                waveTransitionTimer = 0.f;
                currentWave++;
                initEnemies();
                initChests();
            }
            return;
        }

        sf::Vector2f oldPos = player.getPosition();

        player.handleInput();
        player.update(deltaTime);

        sf::Vector2f newPos = player.getPosition();
        if (!isWalkable(newPos.x, newPos.y)) {
            player.setPosition(oldPos.x, oldPos.y);
        }

        for (auto& enemy : enemies) {
            if (!enemy.isDead())
                enemy.update(deltaTime);
        }

        nearbyEnemyIndex = getNearbyEnemyIndex();
        nearbyChestIndex = getNearbyChestIndex();
        nearShopNPC = isNearShopNPC();

        // Animate shop NPC
        if (wizardLoaded) {
            shopNPC.animTimer += deltaTime;
            if (shopNPC.animTimer >= shopNPC.animSpeed) {
                shopNPC.animTimer = 0.f;
                shopNPC.animFrame = (shopNPC.animFrame + 1) % shopNPC.numFrames;
                shopNPC.sprite.setTextureRect(sf::IntRect(
                    shopNPC.animFrame * shopNPC.frameSize, 0,
                    shopNPC.frameSize, shopNPC.frameSize));
            }
        }

        // Update slash animation
        if (playingSlash) {
            slashTimer += deltaTime;
            if (slashTimer >= slashSpeed) {
                slashTimer = 0.f;
                slashFrame++;
                if (slashFrame >= slashTotalFrames) {
                    playingSlash = false;
                }
            }
        }

        // Update chest popup timer
        if (showingChestPopup) {
            chestPopupTimer += deltaTime;
            if (chestPopupTimer >= chestPopupDuration) {
                showingChestPopup = false;
            }
        }

    } else if (state == GameState::BATTLE) {
        battleSystem.update(deltaTime);

        if (battleSystem.isBattleOver()) {
            if (battleSystem.isPlayerDead()) {
                state = GameState::GAME_OVER;
                soundManager.playGameOver();
            } else if (battleSystem.isEnemyDead()) {
                if (currentEnemyIndex >= 0) {
                    sf::Vector2f epos = enemies[currentEnemyIndex].getPosition();
                    slashPosition = sf::Vector2f(epos.x - 24.f, epos.y - 24.f);
                    playingSlash = true;
                    slashFrame = 0;
                    slashTimer = 0.f;
                }

                soundManager.playEnemyDeath();

                bool allDead = true;
                for (auto& e : enemies) {
                    if (!e.isDead()) { allDead = false; break; }
                }
                if (allDead) {
                    if (currentWave >= maxWaves) {
                        state = GameState::WIN;
                        soundManager.playWin();
                    } else {
                        waveTransition = true;
                        waveTransitionTimer = 0.f;
                        state = GameState::PLAYING;
                    }
                } else {
                    state = GameState::PLAYING;
                }
            }
            currentEnemyIndex = -1;
        }
    }
}

void GameManager::drawInteractPrompt(sf::RenderWindow& window) {
    sf::Font* font = uiManager.getFont();
    if (!font) return;

    sf::Text prompt;
    prompt.setFont(*font);
    prompt.setCharacterSize(14);
    prompt.setFillColor(sf::Color::Yellow);

    if (nearbyEnemyIndex >= 0) {
        sf::Vector2f epos = enemies[nearbyEnemyIndex].getPosition();
        prompt.setString("Press E");
        prompt.setPosition(epos.x - 5.f, epos.y - 20.f);
        window.draw(prompt);
    } else if (nearbyChestIndex >= 0) {
        sf::Vector2f cpos = chests[nearbyChestIndex].position;
        prompt.setString("Press E");
        prompt.setPosition(cpos.x - 5.f, cpos.y - 20.f);
        window.draw(prompt);
    } else if (nearShopNPC) {
        prompt.setString("Press E - Shop");
        prompt.setPosition(shopNPC.position.x - 15.f, shopNPC.position.y - 20.f);
        window.draw(prompt);
    }
}

void GameManager::drawSlashAnimation(sf::RenderWindow& window) {
    if (!playingSlash || !slashLoaded) return;
    if (slashFrame >= slashTotalFrames) return;

    slashSprite.setTextureRect(sf::IntRect(slashFrame * 64, 0, 64, 64));
    slashSprite.setPosition(slashPosition);
    window.draw(slashSprite);
}

sf::Color GameManager::getWaveTint() const {
    switch (currentWave) {
        case 1: return sf::Color(255, 255, 255, 255);
        case 2: return sf::Color(220, 200, 180, 255);
        case 3: return sf::Color(180, 170, 200, 255);
        default: return sf::Color(255, 255, 255, 255);
    }
}

void GameManager::drawTilemap(sf::RenderWindow& window) {
    sf::Color tint = getWaveTint();

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            float px = x * TILE_SIZE;
            float py = y * TILE_SIZE;
            int tile = tilemap[y][x];

            if (tilesLoaded) {
                // Always draw grass base
                grassSprite.setPosition(px, py);
                grassSprite.setColor(tint);
                window.draw(grassSprite);

                if (tile == 1) {
                    treeSprite.setPosition(px, py);
                    treeSprite.setColor(tint);
                    window.draw(treeSprite);
                } else if (tile == 2) {
                    waterSprite.setPosition(px, py);
                    waterSprite.setColor(tint);
                    window.draw(waterSprite);
                } else if (tile == 3) {
                    pathSprite.setPosition(px, py);
                    pathSprite.setColor(tint);
                    window.draw(pathSprite);
                } else if (tile == 4) {
                    fenceSprite.setPosition(px, py);
                    fenceSprite.setColor(tint);
                    window.draw(fenceSprite);
                } else if (tile == 5) {
                    // House: only draw the sprite once at the top-left tile
                    if (x == 1 && y == 1) {
                        houseSprite.setPosition(px, py);
                        houseSprite.setColor(tint);
                        window.draw(houseSprite);
                    }
                }
            } else {
                sf::RectangleShape tileFb(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                tileFb.setPosition(px, py);
                if (tile == 0 || tile == 3)
                    tileFb.setFillColor(sf::Color(76, 175, 80));
                else if (tile == 2)
                    tileFb.setFillColor(sf::Color(50, 100, 200));
                else
                    tileFb.setFillColor(sf::Color(46, 125, 50));
                window.draw(tileFb);
            }
        }
    }
}

void GameManager::drawChests(sf::RenderWindow& window) {
    for (auto& chest : chests) {
        if (chest.opened) continue;

        if (chestTextureLoaded) {
            chestSprite.setPosition(chest.position);
            window.draw(chestSprite);
        } else {
            sf::RectangleShape box(sf::Vector2f(48.f, 48.f));
            box.setPosition(chest.position);
            box.setFillColor(sf::Color(180, 130, 50));
            box.setOutlineColor(sf::Color(120, 80, 20));
            box.setOutlineThickness(2.f);
            window.draw(box);
        }
    }
}

void GameManager::drawChestPopup(sf::RenderWindow& window) {
    if (!showingChestPopup) return;

    sf::Font* font = uiManager.getFont();
    if (!font) return;

    // Floating text above player, fades out
    float alpha = 1.f - (chestPopupTimer / chestPopupDuration);
    float yOffset = -30.f - (chestPopupTimer * 30.f); // float upward

    sf::Vector2f ppos = player.getPosition();
    sf::Text popup;
    popup.setFont(*font);
    popup.setCharacterSize(22);
    popup.setFillColor(sf::Color(chestPopupColor.r, chestPopupColor.g,
                                  chestPopupColor.b, (sf::Uint8)(alpha * 255)));
    popup.setString(chestPopupText);
    popup.setPosition(ppos.x - 20.f, ppos.y + yOffset);
    window.draw(popup);
}

void GameManager::drawWaveTransition(sf::RenderWindow& window) {
    if (!waveTransition) return;

    sf::Font* font = uiManager.getFont();
    if (!font) return;

    sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    sf::Text waveText;
    waveText.setFont(*font);
    waveText.setCharacterSize(50);
    waveText.setFillColor(sf::Color(255, 215, 0));
    waveText.setString("Wave " + std::to_string(currentWave) + " Complete!");
    sf::FloatRect bounds = waveText.getLocalBounds();
    waveText.setPosition(640.f - bounds.width / 2.f, 250.f);
    window.draw(waveText);

    sf::Text nextText;
    nextText.setFont(*font);
    nextText.setCharacterSize(28);
    nextText.setFillColor(sf::Color::White);
    nextText.setString("Wave " + std::to_string(currentWave + 1) + " incoming...");
    sf::FloatRect nb = nextText.getLocalBounds();
    nextText.setPosition(640.f - nb.width / 2.f, 340.f);
    window.draw(nextText);

    for (int i = 1; i <= maxWaves; i++) {
        sf::CircleShape dot(10.f);
        dot.setPosition(590.f + (i - 1) * 40.f, 420.f);
        if (i <= currentWave)
            dot.setFillColor(sf::Color(255, 215, 0));
        else
            dot.setFillColor(sf::Color(100, 100, 100));
        dot.setOutlineColor(sf::Color::White);
        dot.setOutlineThickness(2.f);
        window.draw(dot);
    }
}

void GameManager::draw(sf::RenderWindow& window, GameState state) {
    if (state == GameState::PLAYING || state == GameState::BATTLE || state == GameState::SHOPPING) {
        drawTilemap(window);

        // Draw chests
        drawChests(window);

        // Draw shop NPC
        drawShopNPC(window);

        for (auto& enemy : enemies) {
            if (!enemy.isDead()) {
                enemy.draw(window);
                // Draw HP bar above enemy
                sf::Vector2f epos = enemy.getPosition();
                uiManager.drawEnemyHPBar(window, epos.x, epos.y,
                                          enemy.hp, enemy.maxHp);
            }
        }

        player.draw(window);

        drawSlashAnimation(window);

        // Draw interact prompt
        if (state == GameState::PLAYING && !waveTransition)
            drawInteractPrompt(window);

        // Draw chest popup
        drawChestPopup(window);

        uiManager.drawHUD(window, player.hp, player.maxHp, player.xp,
                           player.level, player.score, currentWave, maxWaves);

        if (state == GameState::BATTLE) {
            battleSystem.draw(window);
        }

        if (state == GameState::SHOPPING) {
            uiManager.drawShopPanel(window, player.score, shopSelection);
        }

        if (waveTransition) {
            drawWaveTransition(window);
        }
    }
}

Player& GameManager::getPlayer() {
    return player;
}

UIManager& GameManager::getUIManager() {
    return uiManager;
}

SoundManager& GameManager::getSoundManager() {
    return soundManager;
}

void GameManager::reset() {
    player = Player();
    player.loadTexture("assets/images/player.png");
    player.setPosition(2 * TILE_SIZE, 5 * TILE_SIZE);
    questionManager.resetUsed();
    currentWave = 1;
    waveTransition = false;
    waveTransitionTimer = 0.f;
    showingChestPopup = false;
    initEnemies();
    initChests();
    currentEnemyIndex = -1;
    nearbyEnemyIndex = -1;
    nearbyChestIndex = -1;
    playingSlash = false;
    shopSelection = 0;
    nearShopNPC = false;
}
