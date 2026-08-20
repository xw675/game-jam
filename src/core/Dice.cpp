#include "Dice.hpp"
#include "Balance.hpp"
#include "Rng.hpp"

AttackResult resolveAttack(Rng& rng, int might, int defence, int damageDie) {
    AttackResult result;
    result.roll = rng.roll(20);
    result.total = result.roll + might;

    if (result.roll == 20) {
        result.outcome = AttackOutcome::Critical;
        result.damage = rng.roll(damageDie) + rng.roll(damageDie);
    } else if (result.total >= defence) {
        result.outcome = AttackOutcome::Hit;
        result.damage = rng.roll(damageDie);
    } else if (result.total >= defence - Balance::kGrazeWindow) {
        result.outcome = AttackOutcome::Graze;
        result.damage = Balance::kGrazeDamage;
    }

    return result;
}
