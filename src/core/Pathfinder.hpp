#pragma once
#include "Map.hpp"
#include "Vec2i.hpp"
#include <functional>
#include <vector>

std::vector<Vec2i> findPath(const Map& map, const Vec2i& start, const Vec2i& goal, const std::function<bool(const Vec2i&)>& blocked);
