#include "Hunter.hpp"
#include "World.hpp"
#include "Balance.hpp"
#include "Pathfinder.hpp"

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
        return;
    }

    const std::vector<Vec2i> path = findPath(
        world.map(), m_pos, target.position(),
        [&world](const Vec2i& p) { return !world.isFree(p); });

    if (path.size() >= 2) {
        world.moveEntity(*this, path[1]);
    } else {
        stepToward(world, target.position()); // no route
    }
}

int Hunter::might() const {
    return Balance::kHunterMight;
}

EntityKind Hunter::kind() const {
    return EntityKind::Hunter;
}
