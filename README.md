# SEPAKAT

A solo C++ game jam entry for **Malaysia Day**. Turn-based dungeon roguelike, built on [raylib](https://www.raylib.com/), no game engine, no assets, no scripting layer.

## The game

You break into the archival vaults to steal four tokens: **Malaya**, **Sabah**, **Sarawak**, **Singapore**. Each token you lift heals you to full and buffs your attack, and of course raises the alarm, waking the keep and spawning another harimau hunting you. Take all four and the Hall opens, reach it to get out.

16 September 1963 is the day the four came together, and **sepakat** means agreement, coming to one mind. In the game, every piece you add makes you stronger and makes the room around you more dangerous, union and cost arrive in the same turn. The end screen names the year Singapore left.

Controls: **arrows / WASD** to move (one tile = one turn), walk into a guardian to strike it, **R** to abandon the run and break into a fresh keep. F1–F4 are debug keys (reveal map, grant token, kill monsters, take damage).

## Play it

Grab the zip for your machine from [Releases](https://github.com/xw675/game-jam/releases), unzip, run it. Nothing to install, no compiler needed.

- **Windows** — `SEPAKAT-v0-windows-x64.zip`, double-click `SEPAKAT.exe`. Windows will say "Windows protected your PC" because the exe is not code-signed: **More info -> Run anyway**.
- **macOS (Apple Silicon)** — `SEPAKAT-v0-macos-arm64.zip`, double-click `SEPAKAT.app`. macOS refuses it the first time for the same reason: **System Settings -> Privacy & Security -> Open Anyway**. Or in a terminal, `xattr -dr com.apple.quarantine SEPAKAT.app`.

Both zips are built from the tagged source by [`.github/workflows/release.yml`](.github/workflows/release.yml). The scripts in `packaging/` are what make them self-contained: raylib is linked in or shipped alongside, so neither zip needs Homebrew or MSYS2 on the player's machine.

## Build and run

Requires a C++17 compiler, `make`, and **raylib 6.0** (older version should work too)

```sh
# macOS
brew install raylib
make # -> ./game-jam
./game-jam # random dungeon
./game-jam 12345 # replays seed 12345 exactly

# Linux (raylib via your package manager / pkg-config)
sudo apt install libraylib-dev
make
```

The Makefile detects macOS / Linux / MinGW and picks the right include and link flags, on macOS it asks Homebrew where raylib lives (`brew --prefix raylib`).

```sh
make test # builds src/core/ + Catch2 and runs, no raylib, no window
make re
```

## The library, and what it actually gave me

**raylib 6.0**, instead of SFML, SDL2.

- **SFML**: for larger game, althought it is OOP pattern.
- **SDL2**: more boilerplate, can't make it in 4 days.
- **raylib**: one header, `brew install`, and a small honest surface: open a window, read a key, draw a rectangle, draw text. That's all I need.

**raylib gave me a window, a 60 Hz loop, keyboard input, and `DrawRectangle`/`DrawText`.** Everything else in `src/core/` is mine,  map generation, A\* pathfinding, field of view, the turn engine, the d20 combat maths, the alarm/spawn system.

## Where Week 1 shows up in this code

| Concept | Where |
| --- | --- |
| **const correctness** | `const Vec2i&` parameters throughout, const member functions on every accessor (`Map::at`, `World::state`), the const/non-const `World::player()` pair, `constexpr` free functions in `Vec2i.hpp` and every tuning number in `Balance.hpp` |
| **Ownership / RAII** | `std::vector<std::unique_ptr<Monster>>` owns the monsters, `main` holds `std::unique_ptr<World>` so restart *replaces* the world rather than resetting it, stale state is impossible by construction. `Entity`'s copy operations are `= delete`d, `World` takes `GeneratedMap` by value and `std::move`s it, and `Rng&` is a deliberately non-owning reference. |
| **Class design / polymorphism** | `Entity` (abstract, pure virtual `might()` and `kind()`) -> `Player` / `Monster` -> `Hunter`, with `final` on the leaves and a virtual destructor at the root. `Renderer` is a pure interface implemented by `RaylibRenderer`. `Rng` is an interface with two implementations. |
| **Containers** | `std::vector<Tile>` as a flat 2D grid (index arithmetic, not 2d vector), `std::array<bool, 4>` for carried tokens, `std::deque<std::string>` as a bounded message log, `std::priority_queue` inside A\*, `std::vector<Vec2i>` for token positions and spawn candidates |
| **Patterns** | **dependency inversion**: `Renderer` and `Rng` are both injected interfaces, which is exactly what makes the game testable (`SequenceRng` is a scripted test double for the dice). **Facade**: `World` is the single entry point over map, entities, turns and alarm, `main` calls `playerMove` and never touches an entity directly. |
| **Algorithms** | **A\*** with a Manhattan heuristic and lazy deletion (`Pathfinder.cpp`) for hunter pursuit, **BFS** (`Connectivity.cpp`) to prove every generated dungeon is actually winnable, **Bresenham** line of sight plus a radius sweep for fog of war (`Fov.cpp`), room and corridor generation with rejection of overlapping rooms (`MapGenerator.cpp`) |
| **Tests** | Catch2 in `tests/`, run with `make test`. Four files: dice (every branch of the attack table, with scripted rolls), pathfinder (shortest path, unreachable goal, no walls on the path, the goal is exempt from `blocked`), map generation (**200 seeds, every token and the Hall reachable from the start**), and world (the Hall opens to four tokens and not three, a token can't raise the alarm twice). |

## What I cut

no save system, no inventory or items beyond the tokens, one floor, no audio, **no asset files at all** (everything on screen is a rectangle or a character of raylib's built-in font), no ranged combat, no level editor, no ECS.

Cut during the jam:

- **Three guardian types -> one.** `Wanderer` and `Guard` existed as enum names but only `Hunter` was ever implemented, so I deleted the enumerators. Because no switch in the codebase has a `default:`.
- **Animated turns.** `World::lastTurnMoves()` records every move each turn and is currently written but never read, it's the seam the animation would have hung off.
- **BSP room layout** and a scrolling camera, both cost more than the day had left, so the map shrank to 40×24 with bigger tiles instead.

## What I'd do differently

- **Monster awareness ignores line of sight**: a hunter wakes on distance or alarm level, so it can notice you through a wall.
- **`SequenceRng::roll` doesn't bounds-check** how many rolls a test scripted.
- **Room layout.** Rooms and corridors was the safe choice, BSP would give the vaults more character than corridors joining rectangles.
- the enums in `Types.hpp` don't have the explicit underlying type I had planned.