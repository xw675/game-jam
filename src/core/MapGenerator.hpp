#pragma once
#include "Map.hpp"
#include "Vec2i.hpp"
#include "Rng.hpp"
#include <vector>

struct GeneratedMap {
    Map map;
    Vec2i playerStart; // centre of the first room
    std::vector<Vec2i> tokenPositions;
    Vec2i hall; // the exit, also marked Tile::Hall in `map`
};

// The same Rng state produces the same dungeon.
// `roomAttempts` is how many rooms it TRIES to place, not how many it gets.
GeneratedMap generateMap(Rng& rng, int width, int height, int roomAttempts);
