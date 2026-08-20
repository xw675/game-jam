#pragma once
#include "core/Rng.hpp"
#include <cstddef>
#include <vector>

class SequenceRng final : public Rng {
public:
    explicit SequenceRng(std::vector<int> rolls);

    int roll(int sides) override;

    std::size_t used() const;

private:
    std::vector<int> m_rolls;
    std::size_t m_next = 0;
};
