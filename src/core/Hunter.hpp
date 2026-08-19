#pragma once
#include "Monster.hpp"
#include "Vec2i.hpp"
#include "Types.hpp"

class Hunter final : public Monster {
public:
    explicit Hunter(const Vec2i& pos);

    void takeTurn(World& world) override;
    int might() const override;
    EntityKind kind() const override;
};
