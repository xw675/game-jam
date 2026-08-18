#include "Connectivity.hpp"
#include <queue>
#include <algorithm>

// Breadth-first search
//
// One step of the loop: take a tile off the front of the queue, record it as reachable,
// then look at its four neighbours. Any neighbour that is walkable gets marked 
// and pushed onto the back. When the queue is finished, every tile that was ever popped 
// is exactly the reachable set.
std::vector<Vec2i> reachableFrom(const Map& map, const Vec2i& start) {
    std::vector<Vec2i> result;
    if (!map.inBounds(start) || !map.isWalkable(start)) {
        return result;
    }

    // One slot per tile
    std::vector<bool> visited(map.width() * map.height(), false);
    std::queue<Vec2i> queue;

    auto toIndex = [&](const Vec2i& p) {
        return p.y * map.width() + p.x;
    };

    queue.push(start);
    visited[toIndex(start)] = true;

    // Four direction movement only. The player cannot move diagonally.
    const Vec2i dirs[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    while (!queue.empty()) {
        Vec2i curr = queue.front();
        queue.pop();
        result.push_back(curr);

        for (const auto& dir : dirs) {
            Vec2i next = curr + dir;
            if (map.inBounds(next) && map.isWalkable(next)) {
                int idx = toIndex(next);
                if (!visited[idx]) {
                    visited[idx] = true;
                    queue.push(next);
                }
            }
        }
    }

    return result;
}

bool allReachable(const Map& map, const Vec2i& start, const std::vector<Vec2i>& targets) {
    const auto reachable = reachableFrom(map, start);
    for (const auto& target : targets) {
        if (std::find(reachable.begin(), reachable.end(), target) == reachable.end()) {
            return false;
        }
    }

    return true;
}
