#pragma once

class Rng;

enum class AttackOutcome : unsigned char {
    Miss,
    Graze,
    Hit,
    Critical
};

struct AttackResult {
    AttackOutcome outcome = AttackOutcome::Miss;
    int roll = 0; // the bare d20, kept so the log can show the sum
    int total = 0; // roll + might
    int damage = 0; // 0 on a Miss
};

AttackResult resolveAttack(Rng& rng, int might, int defence, int damageDie);
