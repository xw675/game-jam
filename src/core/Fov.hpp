#pragma once
#include "Map.hpp"
#include "Types.hpp"
#include "Vec2i.hpp"
#include <vector>

bool hasLineOfSight(const Map& map, const Vec2i& from, const Vec2i& to);

// Let last turn's Visible tiles to Remembered, then remarks everything the
// eye can currently reach as Visible. A tile that was ever seen is never Unseen
// again.
void updateVisibility(const Map& map, const Vec2i& eye, int radius, std::vector<Visibility>& out);
