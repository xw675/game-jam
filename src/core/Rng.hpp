#pragma once
#include <random>
#include <cstdint>

class Rng {
public:
    virtual ~Rng() = default;
    virtual int roll(int sides) = 0;
    int range(int lo, int hi);
};

// RNG to generate 32-bit unsigned int
class DefaultRng final : public Rng {
public:
    explicit DefaultRng(std::uint32_t seed);
    int roll(int sides) override;
private:
    std::mt19937 m_engine;
};