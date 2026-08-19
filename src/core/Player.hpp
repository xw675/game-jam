#pragma once
#include "Entity.hpp"
#include "Balance.hpp"
#include "Vec2i.hpp"
#include "Types.hpp"
#include <array>

class Player final : public Entity {
public:
    explicit Player(const Vec2i& pos);

    int might() const override;
    EntityKind kind() const override;

    bool hasToken(int index) const;
    void takeToken(int index);
    int  tokenCount() const;
    bool hasAllTokens() const;

private:
    // initialises all four to false.
    std::array<bool, Balance::kTokenCount> m_taken{};
};
