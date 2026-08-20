#pragma once
#include "Types.hpp"
#include "Tile.hpp"
#include "Vec2i.hpp"
#include "Balance.hpp"
#include <array>
#include <deque>
#include <string>

// Which tokens the player is carrying, in token order. The HUD names each
// token, so it needs the four flags - a count cannot tell one token from another.
using TokenRow = std::array<bool, Balance::kTokenCount>;

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void beginFrame() = 0;
    virtual void drawTile(Tile t, const Vec2i& pos, Visibility vis) = 0;
    virtual void drawEntity(EntityKind kind, const Vec2i& pos, Visibility vis) = 0;
    virtual void drawHud(int hp, int maxHp, const TokenRow& tokens, int alarm) = 0;
    virtual void drawLog(const std::deque<std::string>& lines) = 0;
    // Full-screen overlays. The renderer owns the wording, core owns the state.
    virtual void drawTitle() = 0;
    virtual void drawEndScreen(GameState outcome, const TokenRow& tokens, int alarm) = 0;
    virtual void endFrame() = 0;
};
