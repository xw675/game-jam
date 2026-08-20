#include <catch_amalgamated.hpp>

#include "core/Map.hpp"
#include "core/Pathfinder.hpp"
#include "core/Vec2i.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

bool nothingBlocked(const Vec2i&) {
    return false;
}

void carveLine(Map& map, const Vec2i& a, const Vec2i& b) {
    for (int y = std::min(a.y, b.y); y <= std::max(a.y, b.y); ++y) {
        for (int x = std::min(a.x, b.x); x <= std::max(a.x, b.x); ++x) {
            map.set({x, y}, Tile::Floor);
        }
    }
}

Map makeElbow() {
    Map map(9, 7);
    carveLine(map, {1, 1}, {1, 5});
    carveLine(map, {1, 5}, {7, 5});
    return map;
}

}

TEST_CASE("finds the shortest path through a known corridor", "[pathfinder]") {
    const Map map = makeElbow();
    const std::vector<Vec2i> path = findPath(map, {1, 1}, {7, 5}, nothingBlocked);

    // 4 steps down + 6 steps along = 10 moves, which is 11 tiles including the start.
    REQUIRE(path.size() == 11);
    REQUIRE(path.front() == Vec2i{1, 1});
    REQUIRE(path.back() == Vec2i{7, 5});

    // Each tile is exactly one step from the last
    for (std::size_t i = 1; i < path.size(); ++i) {
        REQUIRE(manhattan(path[i - 1], path[i]) == 1);
    }
}

TEST_CASE("a walled-off goal returns empty instead of hanging", "[pathfinder]") {
    Map map(9, 5);
    map.set({1, 1}, Tile::Floor);
    map.set({7, 3}, Tile::Floor); // a second pocket, no corridor between them

    const std::vector<Vec2i> path = findPath(map, {1, 1}, {7, 3}, nothingBlocked);

    REQUIRE(path.empty());
}

TEST_CASE("no tile on a returned path is a wall", "[pathfinder]") {
    const Map map = makeElbow();
    const std::vector<Vec2i> path = findPath(map, {1, 1}, {7, 5}, nothingBlocked);

    REQUIRE_FALSE(path.empty());
    for (const Vec2i& p : path) {
        REQUIRE(map.isWalkable(p));
    }
}

TEST_CASE("blocked tiles are avoided, but the goal itself is exempt", "[pathfinder]") {
    Map map(9, 3);
    carveLine(map, {1, 1}, {7, 1});

    const auto blockMiddle = [](const Vec2i& p) { return p == Vec2i{4, 1}; };
    REQUIRE(findPath(map, {1, 1}, {7, 1}, blockMiddle).empty());

    const auto blockGoal = [](const Vec2i& p) { return p == Vec2i{7, 1}; };
    REQUIRE(findPath(map, {1, 1}, {7, 1}, blockGoal).size() == 7);
}
