# 🐍 Snake Game Pro

<div align="center">

[![Build & Release](https://github.com/nazyasmin920-web/Snake-Game-/actions/workflows/release.yml/badge.svg)](https://github.com/nazyasmin920-web/Snake-Game-/actions/workflows/release.yml)
[![Latest Release](https://img.shields.io/github/v/release/nazyasmin920-web/Snake-Game-?label=Download&logo=github)](https://github.com/nazyasmin920-web/Snake-Game-/releases/latest)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11-blue?logo=windows)](https://github.com/nazyasmin920-web/Snake-Game-/releases/latest)

**A polished, modern Snake game built in C++17 with SFML 2.6**

[⬇️ Download & Play](#-download--play-no-install) • [🛠 Build from Source](#️-build-from-source) • [🎮 Controls](#-controls) • [✨ Features](#-features)

</div>

---

## ⬇️ Download & Play (No Install)

> **Windows 10/11 — No installation required. Just download, unzip, and double-click.**

1. Go to the [**Releases page →**](https://github.com/nazyasmin920-web/Snake-Game-/releases/latest)
2. Download **`SnakeGame-Windows.zip`**
3. **Extract** the ZIP to any folder
4. Double-click **`SnakeGame.exe`**

All DLLs are bundled — nothing else needed.

---

## 🎮 Controls

| Key | Action |
|:---:|--------|
| `W` / `↑` | Move Up |
| `A` / `←` | Move Left |
| `S` / `↓` | Move Down |
| `D` / `→` | Move Right |
| `1` / `2` | Select Mode (Classic / Time Attack) |
| `M` | Return to Mode Select Menu |
| `Space` | Pause / Resume |
| `Enter` | Start / Restart |
| `Escape` | Main Menu / Quit |

---

## 🕹️ Two Game Modes

### 🟢 Part 1: Classic Mode (The Original Game)
- **Endless Survival:** No countdown timer—play for as long as you can survive!
- **Leveling & Speed:** Every 5 foods (50 pts) advances your level and increases speed.
- **Color Progression:** Snake transforms colors (Emerald, Cyan, Purple, Gold, Crimson at Lvl 5, Rainbow at Lvl 6+).
- **Hazard Obstacles:** Starting at Level 6, random hazard blocks appear near food.
- **Wrap-Around Portals:** Seamless screen wrapping with no border death.

### 🔴 Part 2: Time Attack / Food Quota Challenge Mode
- **Eat Under Pressure:** You must eat a required amount of food before the countdown runs out!
- **Wave Progression:**
  - **Wave 1:** 5 food items in 30 seconds
  - **Wave 2+:** Target quota increases (+2 food each wave) with bonus countdown time awarded!
- **Dynamic Heads-Up Display:**
  - Real-time countdown timer that flashes urgent red when under 10 seconds.
  - Interactive food progress bar showing current quota vs target.
- **Adrenaline Rush:** Test your speed, routing, and reflexes against the clock!

---

## ✨ Features

### 🌈 Level System
- Eat **5 foods (50 pts)** → Level Up!
- Snake **color changes** with every level:
  - **Level 1** — Emerald Neon Green
  - **Level 2** — Electric Cyber Cyan
  - **Level 3** — Royal Amethyst Purple
  - **Level 4** — Radiant Solar Gold
  - **Level 5** — 🔥 Crimson Inferno Dragon
  - **Level 6+** — 🌈 Prismatic Rainbow Chroma
- Snake **speeds up** with each level

### 🌀 Wrap-Around Portals
- **No wall death!** The snake wraps from one side to the other
- Only **self-collision** kills the snake

### ⚠️ Obstacle Blocks (Level 6+)
- Starting at **Level 6**, a hazard block appears near the food on each spawn
- Touching it = instant death (with full death sequence)
- Obstacle **migrates** to the new food location each time you eat

### ⏱️ Survival Timer
- Live `MM:SS` clock displayed in HUD
- Pauses when game is paused
- Shown on Game Over and Win screens

### 💥 Visual Polish
- **Screen shake** on death
- **Particle burst** explosion on death + food collection
- **Floating `+10`** popups on food eaten
- Animated **snake tongue, eyes, and digestion bulge**
- Pulsing glowing **apple** with bob, stem, leaf, and shine
- Sub-pixel **smoothstep interpolation** for 60 FPS butter-smooth motion
- Animated **score ticker** in HUD

### 🔊 Sound
- Procedural PCM audio (no WAV files required)
- Sounds for: eat, game over, level up, pause

### 🏆 High Score
- Saved to `highscore.txt` between sessions

---

## 🛠️ Build from Source

### Prerequisites

| Tool | Version | Notes |
|------|---------|-------|
| CMake | 3.16+ | [cmake.org](https://cmake.org/download/) |
| C++ Compiler | GCC 10+ / MSVC 2019+ | SFML is auto-downloaded |
| Internet | Required once | FetchContent downloads SFML 2.6.2 |

### Steps

```bash
# 1. Clone the repo
git clone https://github.com/YOUR_USERNAME/YOUR_REPO.git
cd YOUR_REPO

# 2. Configure (downloads SFML automatically)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 3. Build
cmake --build build --config Release

# 4. Run
./build/Release/SnakeGame.exe   # Windows
./build/SnakeGame               # Linux/Mac (if supported)
```

> **Note for Windows with MinGW:** If using MinGW/GCC instead of MSVC, add:
> ```
> -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++
> ```

---

## 📂 Project Structure

```
Snake Game/
├── .github/
│   └── workflows/
│       └── release.yml       ← GitHub Actions: auto-build & release
├── assets/
│   ├── fonts/
│   │   └── font.ttf          ← UI font (Ubuntu Regular, OFL)
│   └── sounds/               ← Optional WAV files (fallback to procedural)
├── include/
│   ├── Game.hpp              ← Game loop, state machine, particles
│   ├── Snake.hpp             ← Body, interpolation, animations
│   ├── Food.hpp              ← Placement, pulse animation
│   ├── Obstacle.hpp          ← Hazard block manager
│   ├── UI.hpp                ← HUD, overlays, floating text
│   └── Audio.hpp             ← Sound manager
├── src/
│   ├── main.cpp
│   ├── Game.cpp
│   ├── Snake.cpp
│   ├── Food.cpp
│   ├── Obstacle.cpp
│   ├── UI.cpp
│   └── Audio.cpp
└── CMakeLists.txt            ← Build config (auto-fetches SFML 2.6.2)
```

---

## 🚀 Creating a New Release (for maintainers)

```bash
git tag v1.0.0
git push origin v1.0.0
```

GitHub Actions will automatically:
1. Build the game on Windows
2. Bundle the EXE + all DLLs + assets into a ZIP
3. Publish a **GitHub Release** with download link

---

## 📄 License

This project is released under the **MIT License**.  
SFML is licensed under the [zlib/libpng license](https://www.sfml-dev.org/license.php).  
Font: [Ubuntu Regular](https://fonts.google.com/specimen/Ubuntu) — licensed under the [Ubuntu Font License](https://ubuntu.com/legal/font-licence).

---

<div align="center">
Made with ❤️ in C++ · Built with <a href="https://www.sfml-dev.org/">SFML 2.6</a>
</div>
