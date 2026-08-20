#pragma once

namespace Balance {
    constexpr int kPlayerHp = 10;
    constexpr int kPlayerDefence = 12;
    constexpr int kPlayerMight = 1;
    constexpr int kPlayerDamageDie = 6;

    constexpr int kHunterHp = 6;
    constexpr int kHunterDefence = 11;
    constexpr int kHunterMight = 2;
    constexpr int kHunterDamageDie = 4;
    constexpr int kMonsterSenseRange = 8;

    constexpr int kTokenCount = 4;
    constexpr int kVisionRadius = 6;
    constexpr int kMapWidth = 40;
    constexpr int kMapHeight = 24;
    constexpr int kMinRooms = 6;

    constexpr int kHuntersPerAlarm = 1; // spawned per token, flat
    constexpr int kSpawnMinDistance = 10; // no hunter ever appears in your lap
    constexpr int kAlarmWakesAll = 3; // at this alarm the keep stops sleeping
    constexpr int kTokenHeal = kPlayerHp;

    constexpr int kGrazeWindow = 2; // miss by this much or less and you still scratch
    constexpr int kGrazeDamage = 1; // set to 0 to switch grazes off
}
