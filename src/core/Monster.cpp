#include "Monster.hpp"
#include "World.hpp"
#include "Balance.hpp"

Monster::Monster(const Vec2i& pos, int hp, int defence, int damageDie)
    : Entity(pos, hp, defence, damageDie) {}

bool Monster::isAware() const {
    return m_aware;
}

void Monster::updateAwareness(const World& world) {
    if (m_aware) {
        return;
    }
    if (manhattan(m_pos, world.player().position()) <= Balance::kMonsterSenseRange) {
        m_aware = true;
    }
}

void Monster::stepToward(World& world, const Vec2i& target) {
    const int dx = target.x - m_pos.x;
    const int dy = target.y - m_pos.y;

    const Vec2i stepX{(dx > 0) - (dx < 0), 0};
    const Vec2i stepY{0, (dy > 0) - (dy < 0)};

    const bool xIsLonger = (dx < 0 ? -dx : dx) >= (dy < 0 ? -dy : dy);
    const Vec2i steps[2] = {xIsLonger ? stepX : stepY, xIsLonger ? stepY : stepX};

    for (const Vec2i& step : steps) {
        if (step.x == 0 && step.y == 0) {
            continue;
        }
        const Vec2i next = m_pos + step;
        if (world.isFree(next)) {
            world.moveEntity(*this, next);
            return;
        }
    }
}
