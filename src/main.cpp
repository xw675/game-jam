#include "core/MapGenerator.hpp"
#include "core/Balance.hpp"
#include "core/Rng.hpp"
#include "render/RaylibRenderer.hpp"
#include "raylib.h"

int main() {
    constexpr int kTilePx = 28;
    constexpr int kFontPx = 20;

    DefaultRng rng(std::random_device{}());
    GeneratedMap gen = generateMap(rng, Balance::kMapWidth, Balance::kMapHeight, 120);

    InitWindow(gen.map.width() * kTilePx, gen.map.height() * kTilePx + 40, "My Game");
    SetTargetFPS(60);

    Vec2i playerPos = gen.playerStart;
    RaylibRenderer renderer(kTilePx, kFontPx);

    while (!WindowShouldClose()) {
        Vec2i moveDir{0, 0};
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))    moveDir.y = -1;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))  moveDir.y = 1;
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))  moveDir.x = -1;
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) moveDir.x = 1;

        if (moveDir.x != 0 || moveDir.y != 0) {
            Vec2i nextPos = playerPos + moveDir;
            if (gen.map.isWalkable(nextPos)) {
                playerPos = nextPos;
            }
        }

        renderer.beginFrame();

        for (int y = 0; y < gen.map.height(); ++y) {
            for (int x = 0; x < gen.map.width(); ++x) {
                Vec2i p{x, y};
                renderer.drawTile(gen.map.at(p), p, Visibility::Visible);
            }
        }

        for (const auto& tokenPos : gen.tokenPositions) {
            renderer.drawEntity(EntityKind::Token, tokenPos, Visibility::Visible);
        }

        renderer.drawEntity(EntityKind::Player, playerPos, Visibility::Visible);
        renderer.drawHud(Balance::kPlayerHp, Balance::kPlayerHp, 0, 0);

        renderer.endFrame();
    }

    CloseWindow();
    return 0;
}