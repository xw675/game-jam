#include "Pathfinder.hpp"
#include <algorithm>
#include <queue>

namespace {

struct Node {
    int index;
    int f;
};

// std::priority_queue is a max heap and we want the cheapest f first, so the
// comparator is '>'.
struct CheapestFirst {
    bool operator()(const Node& a, const Node& b) const {
        return a.f > b.f;
    }
};

}

std::vector<Vec2i> findPath(const Map& map, const Vec2i& start, const Vec2i& goal,
                            const std::function<bool(const Vec2i&)>& blocked) {
    std::vector<Vec2i> path;
    if (!map.isWalkable(start) || !map.isWalkable(goal)) {
        return path;
    }

    const int width = map.width();
    const int cellCount = width * map.height();

    const auto toIndex = [width](const Vec2i& p) { return p.y * width + p.x; };
    const auto toPoint = [width](int index) { return Vec2i{index % width, index / width}; };

    // one slot per tile, flat indexed
    std::vector<int> gScore(cellCount, -1);
    std::vector<int> cameFrom(cellCount, -1);
    std::vector<bool> closed(cellCount, false);

    std::priority_queue<Node, std::vector<Node>, CheapestFirst> open;

    const int startIndex = toIndex(start);
    const int goalIndex = toIndex(goal);
    gScore[startIndex] = 0;
    open.push({startIndex, manhattan(start, goal)});

    const Vec2i dirs[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    while (!open.empty()) {
        const Node current = open.top();
        open.pop();

        if (closed[current.index]) {
            continue;
        }
        closed[current.index] = true;

        if (current.index == goalIndex) {
            for (int at = goalIndex; at != -1; at = cameFrom[at]) {
                path.push_back(toPoint(at));
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        const Vec2i here = toPoint(current.index);
        for (const Vec2i& dir : dirs) {
            const Vec2i next = here + dir;
            if (!map.isWalkable(next)) {
                continue;
            }
            const int nextIndex = toIndex(next);
            if (nextIndex != goalIndex && blocked(next)) {
                continue;
            }

            const int tentative = gScore[current.index] + 1;
            if (gScore[nextIndex] != -1 && tentative >= gScore[nextIndex]) {
                continue;
            }
            gScore[nextIndex] = tentative;
            cameFrom[nextIndex] = current.index;
            open.push({nextIndex, tentative + manhattan(next, goal)});
        }
    }

    return path; // empty: no route exists
}
