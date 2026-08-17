#include "core/Map.hpp"
#include "core/Balance.hpp"
#include "render/RaylibRenderer.hpp"
#include "raylib.h"

int main() {
    constexpr int kWidth = 20;
    constexpr int kHeight = 12;
    constexpr int kTilePx = 32;

    InitWindow(kWidth * kTilePx, kHeight * kTilePx + 40, "My Game");
    SetTargetFPS(60);

    Map map(kWidth, kHeight);
    for (int y = 1; y < kHeight -1; ++y) {
        for (int x = 1; x < kWidth - 1; ++x) {
            map.set({x, y}, Tile::Floor);
        }
    }

    Vec2i playerPos{2, 2};
    RaylibRenderer renderer(kTilePx, 20);

    while(!WindowShouldClose()) {
        Vec2i moveDir{0, 0};
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            moveDir.y = -1;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            moveDir.y = 1;
        }
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            moveDir.x = -1;
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            moveDir.x = 1;
        }

        if (moveDir.x != 0 || moveDir.y != 0) {
            Vec2i nextPos = playerPos + moveDir;
            if (map.isWalkable(nextPos)) {
                playerPos = nextPos;
            }
        }

        renderer.beginFrame();

        for (int y = 0; y < map.height(); ++y) {
            for (int x = 0; x < map.width(); ++x) {
                Vec2i p{x, y};
                renderer.drawTile(map.at(p), p, Visibility::Visible);
            }
        }

        renderer.drawEntity(EntityKind::Player, playerPos, Visibility::Visible);
        renderer.drawHud(Balance::kPlayerHp, Balance::kPlayerHp, 0, 0);

        renderer.endFrame();
    }

    CloseWindow();

    return 0;
}