#pragma once
#include "Map.hpp"
#include "Vec2i.hpp"
#include <vector>

// Reachability queries on a Map, to prove a generated dungeon is playable.

// Every walkable tile the player could reach from `start` by repeated up/down/left/right moves. 
// Returns empty if `start` itself is not walkable.
std::vector<Vec2i> reachableFrom(const Map& map, const Vec2i& start);

// True when every tile in `targets` is reachable from `start`.
bool allReachable(const Map& map, const Vec2i& start, const std::vector<Vec2i>& targets);
