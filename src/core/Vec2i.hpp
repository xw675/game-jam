#pragma once

struct Vec2i {
    int x = 0;
    int y = 0;
};

constexpr bool operator==(const Vec2i& a, const Vec2i& b) {
    return a.x == b.x && a.y == b.y;
}
constexpr Vec2i operator+(const Vec2i& a, const Vec2i& b) {
    return {a.x + b.x, a.y + b.y};
}
constexpr int manhattan(const Vec2i& a, const Vec2i& b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
}