#pragma once
#include "Entity.hpp"
#include "Vec2i.hpp"
#include "Types.hpp"

class Player final : public Entity {
public:
    explicit Player(const Vec2i& pos);

    int might() const override;
    EntityKind kind() const override;
};
