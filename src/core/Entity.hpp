#pragma once
#include "Vec2i.hpp"
#include "Types.hpp"

class Entity {
public:
    Entity(const Vec2i& pos, int hp, int defence, int damageDie);
    virtual ~Entity() = default;

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    const Vec2i& position() const;
    int hp() const;
    int defence() const;
    int damageDie() const;
    bool isAlive() const;
    virtual int might() const = 0;
    virtual EntityKind kind() const = 0;

    void moveTo(const Vec2i& p);
    void takeDamage(int amount);

protected:
    Vec2i m_pos{};
    int m_hp = 1;
    int m_defence = 10;
    int m_damageDie = 3;
};
