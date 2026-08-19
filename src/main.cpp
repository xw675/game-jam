#include "core/World.hpp"
#include "core/MapGenerator.hpp"
#include "core/Balance.hpp"
#include "core/Rng.hpp"
#include "render/RaylibRenderer.hpp"
#include "raylib.h"
#include <memory>

namespace {
constexpr int kTilePx = 28;
constexpr int kFontPx = 20;
constexpr int kRoomAttempts = 120;

std::unique_ptr<World> newRun(Rng& rng) {
    return std::make_unique<World>(generateMap(rng, Balance::kMapWidth, Balance::kMapHeight, kRoomAttempts), rng);
}
}

int main() {
    DefaultRng rng(std::random_device{}());

    // unique_ptr, not a plain World: World holds `Rng&` and deletes its copy operations,
    // so it can be created but never reassigned.
    std::unique_ptr<World> world = newRun(rng);

    InitWindow(Balance::kMapWidth * kTilePx,
               Balance::kMapHeight * kTilePx + RaylibRenderer::kHudPx + RaylibRenderer::kLogPx,
               "My Game");
    SetTargetFPS(60);

    RaylibRenderer renderer(kTilePx, kFontPx);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            world = newRun(rng);
        }

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
                renderer.drawTile(map.at(p), p, Visibility::Visible);
            }
        }

        for (const Vec2i& tokenPos : world->tokens()) {
            renderer.drawEntity(EntityKind::Token, tokenPos, Visibility::Visible);
        }

        for (const std::unique_ptr<Monster>& monster : world->monsters()) {
            renderer.drawEntity(monster->kind(), monster->position(), Visibility::Visible);
        }

        renderer.drawEntity(EntityKind::Player, world->player().position(), Visibility::Visible);
        renderer.drawHud(world->player().hp(), Balance::kPlayerHp, 0, 0);
        renderer.drawLog(world->messages());

        renderer.endFrame();
    }

    CloseWindow();
    return 0;
}