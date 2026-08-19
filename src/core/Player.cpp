#include "Player.hpp"
#include "Balance.hpp"
#include <algorithm>
#include <cassert>

Player::Player(const Vec2i& pos)
    : Entity(pos, Balance::kPlayerHp, Balance::kPlayerDefence, Balance::kPlayerDamageDie) {}

int Player::might() const {
    return Balance::kPlayerMight + tokenCount();
}

EntityKind Player::kind() const {
    return EntityKind::Player;
}

bool Player::hasToken(int index) const {
    assert(index >= 0 && index < Balance::kTokenCount);
    return m_taken[static_cast<std::size_t>(index)];
}

void Player::takeToken(int index) {
    assert(index >= 0 && index < Balance::kTokenCount);
    m_taken[static_cast<std::size_t>(index)] = true;
}

int Player::tokenCount() const {
    return static_cast<int>(std::count(m_taken.begin(), m_taken.end(), true));
}

bool Player::hasAllTokens() const {
    return tokenCount() == Balance::kTokenCount;
}
