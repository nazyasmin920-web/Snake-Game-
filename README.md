# 🐍 2D Snake Game Pro (SFML C++)

A modern, real-time graphical **2D Snake Game** built in **C++17** using the **SFML 2.6** graphics and audio library, configured with **CMake**.

Featuring responsive keyboard controls, sub-pixel smoothstep movement interpolation, dramatic death sequence with screen shake and particle explosion, dynamic food digestion ripples, procedural audio, and polished arcade visuals.

---

## 🎮 New & Enhanced Features

- **⏱️ Live Game Timer:**
  - Real-time match survival timer formatted as `MM:SS` (e.g. `01:42`) displayed directly in the top HUD.
  - Automatically pauses when the game is paused.
  - Final survival time is preserved and showcased on the Game Over and Victory screens.
- **🌈 Dynamic Snake Colors & Level 5 Crimson Transformation:**
  - Every 5 food items collected (50 points), the game levels up!
  - With each level, the snake's skin dynamically transforms into a new vibrant color theme:
    - **Level 1:** Emerald Neon Green
    - **Level 2:** Electric Cyber Cyan / Aqua
    - **Level 3:** Royal Amethyst Purple
    - **Level 4:** Radiant Solar Gold / Amber Blaze
    - **Level 5:** 🔥 **Crimson Inferno / Ruby Dragon** (Legendary transformation!)
    - **Level 6+:** Prismatic Cosmic Chroma (smoothly shifts through the entire rainbow spectrum!)
  - Celebratory **"LEVEL UP!"** floating banner and firework spark burst when unlocking new tiers!
- **🌀 Pass-Through Portal Lines (No Death on Borders):**
  - The boundary lines are now **pass-through portals**: when the snake crosses the left, right, top, or bottom border, it smoothly wraps around and re-enters from the opposite side!
  - **The snake NEVER dies from touching the lines.**
  - The snake **ONLY dies if it bites into its own body (self-collision)**.
- **💥 Dramatic Death Sequence:**
  - Screen shake effect on impact with walls or self.
  - Snake head transforms into cartoon stunned **"X"** eyes upon dying.
  - Multi-colored explosive particle burst (emerald, crimson, and gold sparks) cascading from the snake upon collision.
  - Segment flash and collapse transition before displaying the Game Over card.
- **✨ Enhanced Modern Arcade Graphics:**
  - **Organic Snake Body:** Rounded connected segments with soft 3D drop-shadows and specular glossy highlights.
  - **Living Animations:** Animated red forked tongue flicking in the direction of movement. Expressive cartoon eyes that look towards the direction of travel with specular glints.
  - **Food Digestion Ripple:** Swallowed food generates a physical bulge traveling down the snake's body in real time!
  - **Juicy Apple:** Multi-layered pulsing glowing aura, floating idle bobbing, 3D drop shadow, brown stem, and green leaf.
  - **Floating Score Popups:** Floating `+10` indicators rise and fade out on food consumption.
  - **Particle Systems:** Sparkling star bursts on eating food and fireworks on victory.
- **🏎️ Ultra-Smooth Motion:**
  - High-precision fixed-timestep logic decoupled from rendering.
  - **Smoothstep cubic interpolation** ($3\alpha^2 - 2\alpha^3$) eliminates micro-stutter and creates butter-smooth motion at 60 FPS / 144+ FPS.
  - Smooth animated score ticker.
  - Progress bar in the top HUD showing progress towards the next speed level.
- **🕹️ Responsive Controls:**
  - Two-command input queue buffering fast combination turns without dropped keys or accidental 180° neck collisions.
- **🔊 Hybrid Sound System:**
  - Procedural in-memory PCM audio synthesis fallback ensuring audio always works even without external WAV files.

---

## 🕹️ Controls

| Key | Action |
| :--- | :--- |
| **W** / **Up Arrow** | Move Snake Up |
| **A** / **Left Arrow** | Move Snake Left |
| **S** / **Down Arrow** | Move Snake Down |
| **D** / **Right Arrow** | Move Snake Right |
| **Space** | Pause / Resume Game |
| **Enter** | Start / Restart Game |
| **Escape** | Exit Application |

---

## 📂 Project Architecture

```text
Sanke game/
├── CMakeLists.txt          # CMake build configuration with automated SFML FetchContent
├── README.md               # Complete project documentation and guide
├── .gitignore              # Git ignore rules for build artifacts and IDE files
├── assets/
│   ├── fonts/
│   │   └── font.ttf        # Bundled TrueType font for HUD and UI overlays
│   └── sounds/
│       ├── eat.wav         # Crisp rising chirp for food consumption
│       ├── gameover.wav    # Descending buzz for collision
│       ├── pause.wav       # Soft click for pause/unpause
│       └── start.wav       # Bright two-tone chime
├── include/
│   ├── Audio.hpp           # Sound manager with procedural sample synthesis fallback
│   ├── Food.hpp            # Food placement, random generator, and pulse animation
│   ├── Game.hpp            # Master game loop, state machine, particle engine, and camera shake
│   ├── Snake.hpp           # Body segments, smoothstep interpolation, digestion bulges, and death states
│   └── UI.hpp              # HUD rendering, overlays (Start, Pause, Game Over, Win), and floating text
└── src/
    ├── Audio.cpp
    ├── Food.cpp
    ├── Game.cpp
    ├── main.cpp            # Application entry point with exception handling
    ├── Snake.cpp
    └── UI.cpp
```

---

## 🛠️ How to Run the Game

To launch the compiled game at any time:

```powershell
.\build\SnakeGame.exe
```
*(Or double-click `SnakeGame.exe` in the `build` folder in Windows File Explorer).*
