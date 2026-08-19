#include "Player.hpp"
#include "Balance.hpp"

Player::Player(const Vec2i& pos)
    : Entity(pos, Balance::kPlayerHp, Balance::kPlayerDefence, Balance::kPlayerDamageDie) {}

int Player::might() const {
    return Balance::kPlayerMight;
}

EntityKind Player::kind() const {
    return EntityKind::Player;
}
