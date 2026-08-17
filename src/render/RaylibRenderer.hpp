#pragma once
#include "core/Renderer.hpp"

class RaylibRenderer : public Renderer {
public:
    RaylibRenderer(int tilePx, int fontPx);
    void beginFrame() override;
    void drawTile(Tile t, const Vec2i& pos, Visibility vis) override;
    void drawEntity(EntityKind kind, const Vec2i& pos, Visibility vis) override;
    void drawHud(int hp, int maxHp, int seals, int alarm) override;
    void endFrame() override;
private:
    int m_tilePx;
    int m_fontPx;
};