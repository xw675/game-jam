#pragma once
#include "Entity.hpp"
#include "Vec2i.hpp"

class World;

class Monster : public Entity {
public:
    Monster(const Vec2i& pos, int hp, int defence, int damageDie);

    virtual void takeTurn(World& world) = 0;

    bool isAware() const;
    void updateAwareness(const World& world);

protected:
    void stepToward(World& world, const Vec2i& target);

    bool m_aware = false;
};
