#include "RaylibRenderer.hpp"
#include "raylib.h"

namespace {
constexpr int kHudHeight = 40;
}

RaylibRenderer::RaylibRenderer(int tilePx, int fontPx) 
    : m_tilePx(tilePx), m_fontPx(fontPx) {}

void RaylibRenderer::beginFrame() {
    BeginDrawing();
    ClearBackground(BLACK);
}

void RaylibRenderer::drawTile(Tile t, const Vec2i& pos, Visibility vis) {
    if (vis == Visibility::Unseen) {
        return;
    }

    int posX = pos.x * m_tilePx;
    int posY = pos.y * m_tilePx + kHudHeight; //leave space for HUD

    Color color = RAYWHITE;
    switch (t) {
        case Tile::Wall:
            color = DARKGRAY;
            break;
        case Tile::Floor:
            color = RAYWHITE;
            break;
        case Tile::Hall:
            color = BLUE;
            break;
    }

    if (vis == Visibility::Remembered) {
        color = GRAY;
    }

    DrawRectangle(posX, posY, m_tilePx - 1, m_tilePx - 1, color);
}

void RaylibRenderer::drawEntity(EntityKind kind, const Vec2i& pos, Visibility vis) {
    if (vis != Visibility::Visible) {
        return;
    }

    int posX = pos.x * m_tilePx + m_tilePx / 4;
    int posY = pos.y * m_tilePx + kHudHeight + m_tilePx / 8;

    const char* glyph = "?";
    Color color = WHITE;

    switch (kind)
    {
    case EntityKind::Player:
        glyph = "@";
        color = YELLOW;
        break;
    case EntityKind::Wanderer:
        glyph = "W";
        color = ORANGE;
        break;
    case EntityKind::Hunter:
        glyph = "H";
        color = PURPLE;
        break;
    case EntityKind::Guard:
        glyph = "G";
        color = RED;
        break;
    case EntityKind::Token:
        glyph = "$";
        color = GOLD;
        break;
    case EntityKind::Hall:
        glyph = "E";
        color = GREEN;
        break;
    }

    DrawText(glyph, posX, posY, m_fontPx, color);
}

void RaylibRenderer::drawHud(int hp, int maxHp, int tokens, int alarm) {
    DrawText(TextFormat("HP: %d/%d  Tokens: %d/4  Alarm: %d", hp, maxHp, tokens, alarm), 10, 10, 20, GREEN);
}

void RaylibRenderer::endFrame() {
    EndDrawing();
}