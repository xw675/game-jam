#include "RaylibRenderer.hpp"
#include "Strings.hpp"
#include "raylib.h"
#include <cstddef>
#include <string>

namespace {

void drawCentered(const char* text, int y, int size, Color color) {
    DrawText(text, (GetScreenWidth() - MeasureText(text, size)) / 2, y, size, color);
}

const char* glyphFor(EntityKind kind) {
    switch (kind) {
        case EntityKind::Player:
            return "@";
        case EntityKind::Hunter:
            return "H";
        case EntityKind::Token:
            return "$";
        case EntityKind::Hall:
            return "E";
    }
    return "?";
}

Color colorFor(EntityKind kind) {
    switch (kind) {
        case EntityKind::Player:
            return BROWN;
        case EntityKind::Hunter:
            return PURPLE;
        case EntityKind::Token:
            return GOLD;
        case EntityKind::Hall:
            return GREEN;
    }
    return WHITE;
}

void drawLegendRow(EntityKind kind, const char* label, int x, int y, int size) {
    DrawText(glyphFor(kind), x, y, size, colorFor(kind));
    DrawText(label, x + 28, y, size, LIGHTGRAY);
}

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
    int posY = pos.y * m_tilePx + kHudPx; //leave space for HUD

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
    if (vis == Visibility::Unseen) {
        return;
    }

    int posX = pos.x * m_tilePx + m_tilePx / 4;
    int posY = pos.y * m_tilePx + kHudPx + m_tilePx / 8;

    Color color = colorFor(kind);
    if (vis == Visibility::Remembered) {
        color = Fade(color, 0.45f);
    }

    DrawText(glyphFor(kind), posX, posY, m_fontPx, color);
}

void RaylibRenderer::drawHud(int hp, int maxHp, const TokenRow& tokens, int alarm) {
    // HP as a bar
    constexpr int kBarX = 10, kBarY = 8, kBarW = 130, kBarH = 16;
    const int clamped = hp < 0 ? 0 : hp;
    const int filled = maxHp > 0 ? kBarW * clamped / maxHp : 0;
    DrawRectangle(kBarX, kBarY, kBarW, kBarH, Fade(RED, 0.35f));
    DrawRectangle(kBarX, kBarY, filled, kBarH, GREEN);
    DrawRectangleLines(kBarX, kBarY, kBarW, kBarH, DARKGRAY);
    DrawText(TextFormat("%d / %d", clamped, maxHp), kBarX + kBarW + 8, kBarY, 16, RAYWHITE);

    int x = kBarX + kBarW + 80;
    int prevRight = -1;
    for (int i = 0; i < Balance::kTokenCount; ++i) {
        const bool held = tokens[static_cast<std::size_t>(i)];
        const char* label = TextFormat("[ %s ]", Strings::tokenName(i));
        const int width = MeasureText(label, 18);

        // Only two adjacent held tokens get a line between them.
        if (held && prevRight >= 0) {
            DrawRectangle(prevRight, kBarY + kBarH / 2, x - prevRight, 2, GOLD);
        }
        DrawText(label, x, kBarY, 18, held ? GOLD : DARKGRAY);

        prevRight = held ? x + width : -1;
        x += width + 14;
    }

    // alarm
    const int pipX = GetScreenWidth() - 10 - Balance::kTokenCount * 18;
    DrawText("ALARM", pipX - 62, kBarY, 16, LIGHTGRAY);
    for (int i = 0; i < Balance::kTokenCount; ++i) {
        const int px = pipX + i * 18;
        if (alarm > i) {
            DrawRectangle(px, kBarY + 1, 13, 13, RED);
        } else {
            DrawRectangleLines(px, kBarY + 1, 13, 13, DARKGRAY);
        }
    }
    DrawText(Strings::alarmLine(alarm), kBarX, kBarY + kBarH + 8, 16,
             alarm > 0 ? ORANGE : GRAY);
}

void RaylibRenderer::drawLog(const std::deque<std::string>& lines) {
    const int top = GetScreenHeight() - kLogPx + 6;
    int row = 0;
    for (const std::string& line : lines) {
        DrawText(line.c_str(), 10, top + row * (m_fontPx + 2), m_fontPx - 4, LIGHTGRAY);
        ++row;
    }
}

void RaylibRenderer::drawTitle() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.93f));

    const int big = m_fontPx * 3;
    int y = 60;
    drawCentered(Strings::kGameName, y, big, GOLD);
    y += big + 6;
    drawCentered(Strings::kPlace, y, m_fontPx, LIGHTGRAY);
    y += m_fontPx + 4;
    drawCentered(Strings::kTagline, y, m_fontPx, GRAY);

    y += m_fontPx + 40;
    const int left = GetScreenWidth() / 2 - 170;
    drawLegendRow(EntityKind::Player, "you", left, y, m_fontPx);
    y += m_fontPx + 8;
    drawLegendRow(EntityKind::Token, "a token - take all four", left, y, m_fontPx);
    y += m_fontPx + 8;
    drawLegendRow(EntityKind::Hunter,
                  TextFormat("%s - it hunts you", Strings::guardianName(EntityKind::Hunter)),
                  left, y, m_fontPx);
    y += m_fontPx + 8;
    drawLegendRow(EntityKind::Hall, Strings::kHall, left, y, m_fontPx);

    y += m_fontPx + 36;
    for (const char* line : Strings::kControls) {
        drawCentered(line, y, m_fontPx - 2, RAYWHITE);
        y += m_fontPx + 6;
    }

    drawCentered(Strings::kBegin, GetScreenHeight() - 70, m_fontPx, GOLD);
}

void RaylibRenderer::drawEndScreen(GameState outcome, const TokenRow& tokens, int alarm) {
    const bool won = outcome == GameState::Won;

    // Drawn last, over everything: the dungeon stays visible underneath so the
    // player can see what killed them.
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.82f));

    const int big = m_fontPx * 2;
    int y = GetScreenHeight() / 3;
    drawCentered(won ? Strings::kWonHeadline : Strings::kDeadHeadline, y, big,
                 won ? GOLD : RED);
    y += big + 20;

    // The summary names the tokens, for the same reason the HUD does.
    std::string carried;
    for (int i = 0; i < Balance::kTokenCount; ++i) {
        if (!tokens[static_cast<std::size_t>(i)]) {
            continue;
        }
        if (!carried.empty()) {
            carried += ", ";
        }
        carried += Strings::tokenName(i);
    }
    drawCentered(TextFormat("Tokens taken:  %s", carried.empty() ? "none" : carried.c_str()),
                 y, m_fontPx, RAYWHITE);
    y += m_fontPx + 10;
    drawCentered(Strings::alarmLine(alarm), y, m_fontPx, ORANGE);
    y += m_fontPx + 10;

    if (won) {
        drawCentered(Strings::kEpilogue, y, m_fontPx, LIGHTGRAY);
        y += m_fontPx + 10;
    }

    drawCentered(Strings::kAgain, y + 20, m_fontPx, GOLD);
}

void RaylibRenderer::endFrame() {
    EndDrawing();
}