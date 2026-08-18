#include "Rng.hpp"
#include <cassert>

int Rng::range(int lo, int hi) {
    assert(hi >= lo);
    if (lo == hi) return lo;
    int sides = hi - lo + 1;
    return lo + (roll(sides) - 1);
}

DefaultRng::DefaultRng(std::uint32_t seed) : m_engine(seed) {}

int DefaultRng::roll(int sides) {
    assert(sides >= 1);
    std::uniform_int_distribution<int> dist(1, sides);
    return dist(m_engine);
}