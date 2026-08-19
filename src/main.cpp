#include "core/World.hpp"
#include "core/MapGenerator.hpp"
#include "core/Balance.hpp"
#include "core/Rng.hpp"
#include "render/RaylibRenderer.hpp"
#include "raylib.h"
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <random>

namespace {
constexpr int kTilePx = 28;
constexpr int kFontPx = 20;
constexpr int kRoomAttempts = 120;

std::unique_ptr<World> newRun(Rng& rng) {
    return std::make_unique<World>(
        generateMap(rng, Balance::kMapWidth, Balance::kMapHeight, kRoomAttempts), rng);
}

// ./game-jam 12345 replays a dungeon exactly， no argument means a fresh one.
// bug-reproduction tool.
std::uint32_t seedFromArgs(int argc, char** argv) {
    if (argc >= 2) {
        return static_cast<std::uint32_t>(std::strtoul(argv[1], nullptr, 10));
    }
    return static_cast<std::uint32_t>(std::random_device{}());
}
}

int main(int argc, char** argv) {
    const std::uint32_t seed = seedFromArgs(argc, argv);
    DefaultRng rng(seed);

    // unique_ptr, not a plain World: World holds `Rng&` and deletes its copy operations
    std::unique_ptr<World> world = newRun(rng);

    InitWindow(Balance::kMapWidth * kTilePx,
               Balance::kMapHeight * kTilePx + RaylibRenderer::kHudPx + RaylibRenderer::kLogPx,
               TextFormat("My Game - seed %u", seed));
    SetTargetFPS(60);

    RaylibRenderer renderer(kTilePx, kFontPx);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            world = newRun(rng);
        }

        if (IsKeyPressed(KEY_F1)) world->debugRevealMap();
        if (IsKeyPressed(KEY_F2)) world->debugGrantToken();
        if (IsKeyPressed(KEY_F3)) world->debugKillMonsters();
        if (IsKeyPressed(KEY_F4)) world->debugHurtPlayer(5);

        Vec2i moveDir{0, 0};
        if (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W)) moveDir = {0, -1};
        else if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S)) moveDir = {0, 1};
        else if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) moveDir = {-1, 0};
        else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) moveDir = {1, 0};

        world->playerMove(moveDir);

        renderer.beginFrame();

        const Map& map = world->map();
        for (int y = 0; y < map.height(); ++y) {
            for (int x = 0; x < map.width(); ++x) {
                const Vec2i p{x, y};
                renderer.drawTile(map.at(p), p, world->visibilityAt(p));
            }
        }

        // A token is on the map until the player is carrying it. Tokens are
        // landmarks: they never move, so a remembered one stays drawn (dim).
        const std::vector<Vec2i>& tokens = world->tokenPositions();
        for (std::size_t i = 0; i < tokens.size(); ++i) {
            if (!world->player().hasToken(static_cast<int>(i))) {
                renderer.drawEntity(EntityKind::Token, tokens[i], world->visibilityAt(tokens[i]));
            }
        }

        // Monsters are the opposite: they move, so a remembered one would be a
        // lie about where it is now. Drawn only while actually in view.
        for (const std::unique_ptr<Monster>& monster : world->monsters()) {
            if (world->visibilityAt(monster->position()) == Visibility::Visible) {
                renderer.drawEntity(monster->kind(), monster->position(), Visibility::Visible);
            }
        }

        renderer.drawEntity(EntityKind::Player, world->player().position(), Visibility::Visible);
        renderer.drawHud(world->player().hp(), world->player().maxHp(),
                         world->player().tokenCount(), world->alarm());
        renderer.drawLog(world->messages());

        switch (world->state()) {
            case GameState::Won:
                renderer.drawBanner("YOU ESCAPED  -  R to play again");
                break;
            case GameState::Dead:
                renderer.drawBanner("YOU FELL  -  R to play again");
                break;
            case GameState::Title:
            case GameState::Play:
                break;
        }

        renderer.endFrame();
    }

    CloseWindow();
    return 0;
}