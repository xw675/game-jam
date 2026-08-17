#include "Map.hpp"
#include <cassert>

Map::Map(int width, int height) : m_width(width), m_height(height), m_tiles(width * height, Tile::Wall) {}

int Map::width() const {
    return m_width;
}

int Map::height() const {
    return m_height;
}

bool Map::inBounds(const Vec2i& p) const {
    return p.x >= 0 && p.x < m_width && p.y >= 0 && p.y < m_height;
}

Tile Map::at(const Vec2i& p) const {
    assert(inBounds(p));
    return m_tiles[p.y * m_width + p.x];
}

bool Map::isWalkable(const Vec2i& p) const {
    return inBounds(p) && at(p) != Tile::Wall;
}

void Map::set(const Vec2i& p, Tile t) {
    assert(inBounds(p));
    m_tiles[p.y * m_width + p.x] = t;
}