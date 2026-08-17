#pragma once
#include <vector>
#include "Vec2i.hpp"
#include "Tile.hpp"

class Map {
    public:
    Map(int width, int height);

    int width() const;
    int height() const;
    bool inBounds(const Vec2i& p) const;
    Tile at(const Vec2i& p) const;
    bool isWalkable(const Vec2i& p) const;
    void set(const Vec2i& p, Tile t);

    private:
    int m_width = 0;
    int m_height = 0;
    std::vector<Tile> m_tiles;
};