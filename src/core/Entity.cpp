#include "Entity.hpp"

Entity::Entity(const Vec2i& pos, int hp, int defence, int damageDie)
    : m_pos(pos), m_hp(hp), m_maxHp(hp), m_defence(defence), m_damageDie(damageDie) {}

const Vec2i& Entity::position() const {
    return m_pos;
}

int Entity::hp() const {
    return m_hp;
}

int Entity::maxHp() const {
    return m_maxHp;
}

int Entity::defence() const {
    return m_defence;
}

int Entity::damageDie() const {
    return m_damageDie;
}

bool Entity::isAlive() const {
    return m_hp > 0;
}

void Entity::moveTo(const Vec2i& p) {
    m_pos = p;
}

void Entity::takeDamage(int amount) {
    m_hp -= amount;
    if (m_hp < 0) {
        m_hp = 0;
    }
}

void Entity::heal(int amount) {
    if (!isAlive()) {
        return;
    }
    m_hp += amount;
    if (m_hp > m_maxHp) {
        m_hp = m_maxHp;
    }
}
