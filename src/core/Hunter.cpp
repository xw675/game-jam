#include "Hunter.hpp"
#include "World.hpp"
#include "Balance.hpp"

Hunter::Hunter(const Vec2i& pos)
    : Monster(pos, Balance::kHunterHp, Balance::kHunterDefence, Balance::kHunterDamageDie) {}

void Hunter::takeTurn(World& world) {
    updateAwareness(world);
    if (!m_aware) {
        return;
    }

    Player& target = world.player();

    // Manhattan distance 1 = adjacent
    if (manhattan(m_pos, target.position()) == 1) {
        world.attack(*this, target);
    } else {
        stepToward(world, target.position());
    }
}

int Hunter::might() const {
    return Balance::kHunterMight;
}

EntityKind Hunter::kind() const {
    return EntityKind::Hunter;
}
