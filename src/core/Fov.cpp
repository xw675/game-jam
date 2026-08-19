#include "Fov.hpp"
#include <algorithm>
#include <cstddef>

namespace {

bool blocksSight(const Map& map, const Vec2i& p) {
    return map.at(p) == Tile::Wall;
}

}

bool hasLineOfSight(const Map& map, const Vec2i& from, const Vec2i& to) {
    if (!map.inBounds(from) || !map.inBounds(to)) {
        return false;
    }

    // Bresenham's line algorithm
    // dx and dy are lengths (always >= 0), stepX and stepY carry the direction.
    int x = from.x;
    int y = from.y;
    const int dx = std::abs(to.x - from.x);
    const int dy = std::abs(to.y - from.y);
    const int stepX = (from.x < to.x) ? 1 : -1;
    const int stepY = (from.y < to.y) ? 1 : -1;

    // error is the running comparison
    int error = dx - dy;

    while (x != to.x || y != to.y) {
        const int doubled = 2 * error;
        if (doubled > -dy) { error -= dy; x += stepX; }
        if (doubled <  dx) { error += dx; y += stepY; }

        // Arrived
        if (x == to.x && y == to.y) {
            return true;
        }
        if (blocksSight(map, Vec2i{x, y})) {
            return false;
        }
    }
    return true; // from == to
}

void updateVisibility(const Map& map, const Vec2i& eye, int radius,
                      std::vector<Visibility>& out) {
    const int width = map.width();
    const int height = map.height();
    out.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height),
               Visibility::Unseen);

    // What was lit last turn is now only remembered. Unseen stays Unseen.
    for (Visibility& v : out) {
        if (v == Visibility::Visible) {
            v = Visibility::Remembered;
        }
    }

    if (!map.inBounds(eye)) {
        return;
    }

    const int minX = std::max(0, eye.x - radius);
    const int maxX = std::min(width  - 1, eye.x + radius);
    const int minY = std::max(0, eye.y - radius);
    const int maxY = std::min(height - 1, eye.y + radius);

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            const int dx = x - eye.x;
            const int dy = y - eye.y;

            // Squared Euclidean distance, so the lit area is a disc rather than the diamond manhattan()
            if (dx * dx + dy * dy > radius * radius) {
                continue;
            }

            const Vec2i p{x, y};
            if (hasLineOfSight(map, eye, p)) {
                out[static_cast<std::size_t>(y * width + x)] = Visibility::Visible;
            }
        }
    }
}
