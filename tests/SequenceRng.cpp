#include "SequenceRng.hpp"
#include <cassert>
#include <utility>

SequenceRng::SequenceRng(std::vector<int> rolls) : m_rolls(std::move(rolls)) {}

int SequenceRng::roll(int sides) {

    const int value = m_rolls[m_next];
    ++m_next;

    assert(value >= 1 && value <= sides && "SequenceRng: scripted value out of range for this die");
    (void)sides;

    return value;
}

std::size_t SequenceRng::used() const {
    return m_next;
}
