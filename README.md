# EduQuest RPG

A 2D top-down educational role-playing game built in **C++17** with **SFML 2.5+**. Players explore a tile-based world, get ambushed by patrolling enemies, and fight them off by correctly answering Computer Science, Programming, and Science questions. The better you answer, the faster enemies fall — and the deeper you get into the dungeon.

## Table of Contents

- [Gameplay Overview](#gameplay-overview)
- [Features](#features)
- [Controls](#controls)
- [Screenshots](#screenshots)
- [Requirements](#requirements)
- [Building & Running](#building--running)
- [Project Structure](#project-structure)
- [Architecture](#architecture)
- [Game Data](#game-data)
- [Asset Credits](#asset-credits)
- [License](#license)

## Gameplay Overview

You control a lone adventurer navigating a 30x20 tile map. Enemies patrol fixed routes between two waypoints; wander into their detection radius and the game drops into **Battle mode**. Combat is turn-based and trivia-driven: you're shown a multiple-choice question, and your answer decides the outcome of that exchange.

- **Correct answer** → you land a strike on the enemy and take no damage.
- **Wrong answer** → the enemy hits you instead.
- Defeat an enemy to earn XP and score. Level up every 50 XP, which raises your max HP.
- Clear all enemies in a wave to advance; there are **3 waves**, each harder than the last.
- Open chests scattered around the map for HP or score bonuses.
- Visit the shop NPC to spend earned score on upgrades.
- Die and it's **Game Over**; clear all 3 waves and you **Win**.

## Features

- **Tile-based overworld** with collision, patrolling enemies, treasure chests, and a shop NPC
- **Turn-based battle system** driven by a 45-question bank spanning Computer Science, Programming, and Science at Easy/Medium/Hard difficulty
- **4 enemy types** (Slime, Skeleton, Orc Warrior, Orc Shaman) with independent patrol behavior
- **Leveling system** — XP, score, HP scaling, and a power-boost mechanic
- **Pause menu** and **main menu** with keyboard navigation
- **Animated sprites** for the player, enemies, slash effects, and UI elements (health/XP bars, buttons, panels, cursors)
- **Full audio layer** — background music plus sound effects for correct/wrong answers, level-ups, battle start, enemy death, game over, and victory
- Question bank is non-repeating per session (`QuestionManager` tracks used questions until reset)

## Controls

| Context | Key | Action |
|---|---|---|
| Overworld | `W` `A` `S` `D` | Move player |
| Overworld | `E` | Interact (open chest / talk to shop NPC) |
| Overworld | `Esc` | Pause game |
| Battle | `A` `B` `C` `D` | Select answer option |
| Main Menu | `←` `→` | Change menu selection |
| Main Menu | `Enter` | Confirm selection |
| Pause Menu | `W`/`↑`, `S`/`↓` | Navigate options |
| Pause Menu | `Enter` | Confirm |
| Pause Menu | `Esc` | Resume |
| Game Over / Win | `Enter` | Return to menu |
|  | `Q` | Quit |

## Screenshots

_Add gameplay screenshots or a GIF here — e.g. `assets/images/...` or a `docs/` image you upload separately — to show the overworld, a battle prompt, and the HUD in action._

## Requirements

- **g++** with C++17 support
- **SFML 2.5+** (`graphics`, `window`, `system`, `audio` modules)
- **GNU Make**
- Linux (tested on Ubuntu); should build on any platform with the above installed

Install SFML on Debian/Ubuntu:

```bash
sudo apt install libsfml-dev
```

## Building & Running

```bash
git clone https://github.com/marcodlzo/eduquest-rpg.git
cd eduquest-rpg
make
./eduquest
```

Run `make clean` to remove build artifacts (`src/*.o`, the `eduquest` binary) before a fresh rebuild.

## Project Structure

```
eduquest-rpg/
├── Makefile                  # Build configuration (g++, C++17, SFML linking)
├── LICENSE                   # MIT license
├── include/
│   └── nlohmann/json.hpp     # Vendored single-header JSON parser
├── src/                      # All game source (.cpp/.h)
│   ├── main.cpp              # Entry point
│   ├── Game.{h,cpp}          # Window, main loop, top-level state machine
│   ├── GameManager.{h,cpp}   # Orchestrates overworld, chests, shop, wave logic
│   ├── Player.{h,cpp}        # Movement, stats, animation, leveling
│   ├── Enemy.{h,cpp}         # Patrol AI, detection radius, enemy types
│   ├── BattleSystem.{h,cpp}  # Question-driven combat resolution
│   ├── QuestionManager.{h,cpp} # Loads/serves questions from JSON
│   ├── UIManager.{h,cpp}     # HUD, menus, battle overlay rendering
│   └── SoundManager.{h,cpp}  # Sound effect and music playback
├── data/
│   └── questions.json        # 45 quiz questions (category/difficulty tagged)
└── assets/
    ├── fonts/                # UI font
    ├── images/                # Sprites, tiles, UI kit (bars, buttons, panels)
    └── sounds/                # SFX and background music
```

## Architecture

- **`Game`** owns the `sf::RenderWindow` (1280x720), the frame clock, and the top-level `GameState` machine (`MAIN_MENU → PLAYING → BATTLE → SHOPPING / PAUSED → GAME_OVER / WIN`).
- **`GameManager`** drives everything inside the `PLAYING`/`BATTLE`/`SHOPPING` states: the tilemap, player, enemy list, chests, the shop NPC, wave progression (`currentWave` / `maxWaves = 3`), and collision/trigger checks.
- **`Player`** handles WASD movement (150 px/s), sprite animation (6-frame walk cycles per direction), and RPG stats — HP/max HP, score, XP, level, and a `powerBoost` flag.
- **`Enemy`** patrols between two waypoints at 60 px/s and triggers a battle when the player enters its detection radius. Each enemy has an `EnemyType` (`SLIME`, `SKELETON`, `ORC_WARRIOR`, `ORC_SHAMAN`).
- **`BattleSystem`** resolves combat: reads a key press (`A`/`B`/`C`/`D`), compares it to the active `Question`'s `correctAnswer`, and applies damage to whichever side answered wrong.
- **`QuestionManager`** loads `data/questions.json` into a vector of `Question` structs and serves random, non-repeating questions filtered by category/difficulty, tracking used indices in a `std::set`.
- **`UIManager`** renders the HUD (HP/XP bars, level, score), all menus, and the battle question/answer panel.
- **`SoundManager`** owns playback for background music and every gameplay sound effect.

## Game Data

`data/questions.json` holds the entire question bank — 45 questions tagged by `category` (`Computer Science`, `Programming`, `Science`) and `difficulty` (`Easy`, `Medium`, `Hard`). Each entry follows this shape:

```json
{
  "question": "Which loop runs at least once?",
  "optionA": "for",
  "optionB": "while",
  "optionC": "do-while",
  "optionD": "foreach",
  "answer": "C",
  "category": "Programming",
  "difficulty": "Easy"
}
```

Add or edit questions directly in this file — `QuestionManager` picks them up at load time, no code changes required.

## Asset Credits

Sprite and tile assets are sourced from free game-asset packs (Cute Fantasy, Pixel Crawler, and a UI kit for HUD elements); see license files bundled under the asset folders where present. The JSON parser (`include/nlohmann/json.hpp`) is the vendored single-header build of [nlohmann/json](https://github.com/nlohmann/json).

## License

Released under the [MIT License](LICENSE).
