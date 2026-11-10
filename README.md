# 🐍 2D Snake Game in C++ (SFML)

A complete, modern, real-time graphical **2D Snake Game** built in **C++17** using the **SFML 2.6** graphics and audio library, configured with **CMake**.

Featuring responsive keyboard controls, a fixed-timestep game loop with smooth visual interpolation, dynamic procedural sound effects, animated food, persistent high scores, and polished game states.

---

## 🎮 Features

- **Graphical Window:** Rendered in a crisp 800×600 window with 20px grid cells (40×30 cells).
- **Smooth Gameplay:** Fixed-timestep game loop (grid logic) separated from 60 FPS rendering with smooth linear visual interpolation.
- **Responsive Controls:** Buffered input queue for **W, A, S, D** and **Arrow Keys** preventing lost turns or accidental 180° neck collisions.
- **Audio System:** Built-in retro audio effects (Eat, Game Over, Start, Pause) with procedural in-memory synthesis fallback if WAV files are missing.
- **Food System:** Guaranteed collision-free random food placement with breathing pulse animation and victory trigger on board completion.
- **Score & High Score:** Tracks current score, persistent high score (saved to `highscore.txt`), and progressive speed scaling.
- **Game States:** Start Screen, Active Gameplay, Pause/Resume, Game Over, and Win screens.
- **Zero-Hassle Dependency Management:** Automatically downloads and configures SFML 2.6.2 via CMake's `FetchContent` if no local SFML install is present.

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
│   │   └── font.ttf        # Game typography (bundled TrueType font)
│   └── sounds/
│       ├── eat.wav         # Crisp rising chirp for food consumption
│       ├── gameover.wav    # Descending buzz for collision
│       ├── pause.wav       # Soft click for pause/unpause
│       └── start.wav       # Bright two-tone chime
├── include/
│   ├── Audio.hpp           # Sound manager with procedural sample synthesis fallback
│   ├── Food.hpp            # Food placement, random generator, and pulse animation
│   ├── Game.hpp            # Master game loop, state machine, and window event coordinator
│   ├── Snake.hpp           # Body segments, input queue, step logic, and self-collision
│   └── UI.hpp              # HUD rendering, overlays (Start, Pause, Game Over, Win), and grid
└── src/
    ├── Audio.cpp
    ├── Food.cpp
    ├── Game.cpp
    ├── main.cpp            # Application entry point with exception handling
    ├── Snake.cpp
    └── UI.cpp
```

### Module Responsibilities

1. **`Game` (`include/Game.hpp`, `src/Game.cpp`)**
   - Coordinates window creation, event polling, timing accumulator, and top-level game state transitions (`Start`, `Playing`, `Paused`, `GameOver`, `Win`).
   - Manages persistent high score reading/writing to `highscore.txt`.
   - Controls game difficulty and speed scaling based on food collected.

2. **`Snake` (`include/Snake.hpp`, `src/Snake.cpp`)**
   - Stores snake coordinates using `std::deque<sf::Vector2i>`.
   - Stores previous frame coordinates (`m_prevBody`) to enable smooth visual interpolation.
   - Implements a 2-command buffered input queue (`queueDirection`) to ensure fast, consecutive key presses are never dropped while preventing suicidal reverse turns.
   - Employs accurate tail-vacation rules: when moving into the cell the tail is leaving, no self-collision occurs unless the snake is actively growing.

3. **`Food` (`include/Food.hpp`, `src/Food.cpp`)**
   - Uses C++ `<random>` (`std::mt19937`) to select from currently unoccupied grid coordinates.
   - Detects full-board completion and triggers the victory state.
   - Updates sinusoidal pulse animation for a breathing arcade visual effect.

4. **`UI` (`include/UI.hpp`, `src/UI.cpp`)**
   - Draws the top HUD (Score, High Score, Level indicator) and play arena grid.
   - Renders game state cards (Start Screen, Pause Screen, Game Over Screen, Win Screen).
   - Graceful fallback: if no `.ttf` font can be located on the system, falls back to a procedural vector font renderer so the game never crashes.

5. **`Audio` (`include/Audio.hpp`, `src/Audio.cpp`)**
   - Loads WAV assets from `assets/sounds/` or generates 16-bit PCM waveform samples directly in memory via `sf::SoundBuffer::loadFromSamples`.
   - Gracefully handles missing audio hardware without terminating the application.

---

## 🧠 Key C++ Concepts & Design Highlights

1. **Fixed Timestep with Visual Interpolation:**
   - Grid movements happen at discrete time intervals (e.g. every `0.12s`).
   - The rendering runs uninhibited at 60 FPS.
   - By calculating `alpha = accumulator / stepTime`, the snake's visual position is linearly interpolated:
     $$\text{renderPos} = \text{prevPos} \cdot (1 - \alpha) + \text{currPos} \cdot \alpha$$
     This guarantees silky-smooth movement without altering grid collision logic.

2. **Tail-Vacation Self-Collision Logic:**
   - When a snake does not eat food, its tail cell becomes empty in the same step the head advances.
   - Checking collision against `m_body[0 ... size - 2]` allows the snake to follow its own tail into tight loops without a false game-over.

3. **Input Buffering Queue:**
   - Casual players often press a secondary direction before the next grid tick finishes. By holding up to two valid directional inputs in a `std::queue<Direction>`, turns feel instantaneous and responsive.

4. **Procedural Sound Wave Synthesis:**
   - Instead of hard-failing when an external WAV file is absent, trigonometric waveform generation ($\sin(2\pi f t)$) with exponential amplitude decay builds audio buffers directly into memory.

---

## 🛠️ Build and Run Instructions (Windows)

### Prerequisites

Ensure you have one of the following development toolchains installed:
- **Visual Studio 2019 / 2022** (with the *Desktop development with C++* workload) — **Recommended**
- **VS Code** with the *C/C++* and *CMake Tools* extensions.
- **MinGW-w64** (GCC 9.0+) with CMake 3.16+.

---

### Method 1: Using Visual Studio 2022 (Easiest)

1. Launch **Visual Studio 2022**.
2. Select **Open a local folder** and navigate to:
   `c:\Users\Naz Yasmin\OneDrive\Desktop\Sanke game`
3. Visual Studio will automatically detect `CMakeLists.txt` and generate the CMake cache.
   - *Note: CMake will automatically download and link SFML 2.6.2 via `FetchContent`.*
4. Select `SnakeGame.exe` as the startup item from the top toolbar dropdown.
5. Press **F5** (or the green Play button) to build and run the game!

---

### Method 2: Command Line (Developer PowerShell / Command Prompt)

1. Open **Developer Command Prompt for VS 2022** (or standard PowerShell if `cmake` and your compiler are in PATH):
2. Navigate to the project directory:
   ```cmd
   cd "c:\Users\Naz Yasmin\OneDrive\Desktop\Sanke game"
   ```
3. Configure the build directory:
   ```cmd
   cmake -S . -B build
   ```
   *(CMake will configure the project and fetch SFML 2.6.2 automatically)*
4. Compile the project in Release mode:
   ```cmd
   cmake --build build --config Release
   ```
5. Launch the game:
   ```cmd
   .\build\Release\SnakeGame.exe
   ```
   *(Or `.\build\SnakeGame.exe` depending on generator)*

---

### Method 3: Using a Pre-Installed SFML Directory (Optional)

If you already downloaded pre-compiled SFML 2.6 binaries from [sfml-dev.org](https://www.sfml-dev.org/download.php):
1. Extract SFML (e.g. to `C:\SFML-2.6.2`).
2. Run CMake with the `SFML_DIR` flag:
   ```cmd
   cmake -S . -B build -DSFML_DIR="C:/SFML-2.6.2/lib/cmake/SFML"
   cmake --build build --config Release
   ```

---

## 🔍 Troubleshooting Common Windows Issues

| Issue | Cause | Solution |
| :--- | :--- | :--- |
| **Missing SFML DLLs (`sfml-graphics-2.dll`, etc.)** | Dynamic libraries were not copied next to the executable. | Our `CMakeLists.txt` has a post-build command that automatically copies runtime DLLs. If running manually, copy all `.dll` files from SFML's `bin/` folder into `build/Release/`. |
| **Missing `openal32.dll`** | Audio driver library missing from binary path. | Copy `openal32.dll` (located in SFML's `bin/` or `_deps/sfml-build/extlibs/bin/`) to the executable directory. |
| **Text is invisible or looks blocky** | Missing font or corrupted font file. | The game includes `assets/fonts/font.ttf` and automatically checks `C:/Windows/Fonts/arial.ttf` as a system backup. In the worst case, the built-in procedural vector font activates automatically. |
| **Window closes immediately on launch** | Missing assets or unhandled crash. | Check the console output or verify that the `assets/` directory exists in the folder from which `SnakeGame.exe` is run. |

---

## 📋 Testing Verification Checklist

- [x] Game renders inside an 800×600 graphical window (not terminal).
- [x] W, A, S, D and Arrow keys control the snake smoothly.
- [x] Direct reverse turn into neck is safely blocked.
- [x] Input buffer handles rapid double-turns reliably.
- [x] Snake moves at fixed grid speed with smooth 60 FPS visual interpolation.
- [x] Snake grows by 1 segment upon eating food.
- [x] Food never spawns inside the snake.
- [x] Score increments by 10 per food and high score updates.
- [x] Wall collision and self-collision trigger Game Over.
- [x] Tail leaving cell allows valid moves on non-growth ticks.
- [x] Space pauses and resumes gameplay.
- [x] Enter starts from menu and restarts from Game Over / Win.
- [x] Board completion triggers Victory screen.
- [x] High score persists to `highscore.txt`.
- [x] Clean window close with ESC or 'X' button.
- [x] CMake build system automatically fetches SFML if needed.

