#pragma once
#include "Types.hpp"
#include "Tile.hpp"
#include "Vec2i.hpp"
#include <deque>
#include <string>

class Renderer {
    public:
    virtual ~Renderer() = default;
    virtual void beginFrame() = 0;
    virtual void drawTile(Tile t, const Vec2i&pos, Visibility vis) = 0;
    virtual void drawEntity(EntityKind kind, const Vec2i& pos, Visibility vis) = 0;
    virtual void drawHud(int hp, int maxHp, int tokens, int alarm) = 0;
    virtual void drawLog(const std::deque<std::string>& lines) = 0;
    // Full-screen overlay for the end of a run.
    virtual void drawBanner(const char* text) = 0;
    virtual void endFrame() = 0;
};