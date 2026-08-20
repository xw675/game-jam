#pragma once
#include "core/Renderer.hpp"

class RaylibRenderer : public Renderer {
public:
    static constexpr int kHudPx = 56; // strip above the map: bar, token row, alarm
    static constexpr int kLogPx = 72; // strip below the map, 3 lines

    RaylibRenderer(int tilePx, int fontPx);
    void beginFrame() override;
    void drawTile(Tile t, const Vec2i& pos, Visibility vis) override;
    void drawEntity(EntityKind kind, const Vec2i& pos, Visibility vis) override;
    void drawHud(int hp, int maxHp, const TokenRow& tokens, int alarm) override;
    void drawLog(const std::deque<std::string>& lines) override;
    void drawTitle() override;
    void drawEndScreen(GameState outcome, const TokenRow& tokens, int alarm) override;
    void endFrame() override;
private:
    int m_tilePx;
    int m_fontPx;
};