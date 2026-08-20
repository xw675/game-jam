#include <catch_amalgamated.hpp>

#include "SequenceRng.hpp"
#include "core/Balance.hpp"
#include "core/Dice.hpp"

TEST_CASE("a natural 20 crits and rolls the damage die twice", "[dice]") {
    // Defence 99 is unreachable by roll + might, only the nat-20 rule can hit it.
    SequenceRng rng{{20, 3, 4}};
    const AttackResult r = resolveAttack(rng, 0, 99, 6);

    REQUIRE(r.outcome == AttackOutcome::Critical);
    REQUIRE(r.damage == 7);   // 3 + 4, both damage dice
    REQUIRE(rng.used() == 3); // d20 + two damage dice, and nothing else
}

TEST_CASE("meeting defence is a hit and rolls one damage die", "[dice]") {
    SequenceRng rng{{11, 5}};
    const AttackResult r = resolveAttack(rng, 1, 12, 6); // 11 + 1 == 12

    REQUIRE(r.outcome == AttackOutcome::Hit);
    REQUIRE(r.damage == 5);
    REQUIRE(rng.used() == 2);
}

TEST_CASE("missing by no more than the graze window still scratches", "[dice]") {
    SequenceRng rng{{12 - Balance::kGrazeWindow}}; // the worst roll that still grazes
    const AttackResult r = resolveAttack(rng, 0, 12, 6);

    REQUIRE(r.outcome == AttackOutcome::Graze);
    REQUIRE(r.damage == Balance::kGrazeDamage);
    REQUIRE(r.damage > 0); // a graze is never a wasted turn
    REQUIRE(rng.used() == 1); // a graze is flat damage, so no damage die is rolled
}

TEST_CASE("missing by more than the graze window does nothing", "[dice]") {
    SequenceRng rng{{12 - Balance::kGrazeWindow - 1}};
    const AttackResult r = resolveAttack(rng, 0, 12, 6);

    REQUIRE(r.outcome == AttackOutcome::Miss);
    REQUIRE(r.damage == 0);
    REQUIRE(rng.used() == 1);
}
