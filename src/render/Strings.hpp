#pragma once
#include "core/Types.hpp"

namespace Strings {

inline constexpr const char* kGameName = "SEPAKAT";
inline constexpr const char* kPlace = "Sultan Abdul Samad Archival Vaults";
inline constexpr const char* kHall = "Stadium Merdeka Exit";
inline constexpr const char* kTagline = "Four tokens. One way out.";

inline constexpr const char* const kControls[] = {
    "Arrows or WASD  -  move one tile, and spend one turn",
    "Walk into a guardian  -  strike it",
    "R  -  abandon the run, break into a new keep",
};

inline constexpr const char* kBegin = "Press any key to begin";
inline constexpr const char* kAgain = "Press R to play again";

inline constexpr const char* kWonHeadline  = "YOU ARE OUT";
inline constexpr const char* kDeadHeadline = "YOU FELL";

// 16 September 1963 is Malaysia Day. Singapore left in 1965.
inline constexpr const char* kEpilogue =
    "16 September 1963. Two years later, one of the four would leave.";

inline const char* tokenName(int index) {
    switch (index) {
        case 0: return "Malaya";
        case 1: return "Sabah";
        case 2: return "Sarawak";
        case 3: return "Singapore";
        default: return "?";
    }
}

inline const char* guardianName(EntityKind kind) {
    switch (kind) {
        case EntityKind::Hunter:
            return "Harimau";
        case EntityKind::Player:
            return "You";
        case EntityKind::Token:
            return "Token";
        case EntityKind::Hall:
            return kHall;
    }
    return "?";
}

// The alarm is a sentence, index is the alarm level.
inline const char* alarmLine(int alarm) {
    switch (alarm) {
        case 0:
            return "The vaults are quiet. For now.";
        case 1:
            return "The vaults stir. Something is awake.";
        case 2:
            return "A harimau has your scent.";
        case 3:
            return "They are all hunting you now.";
        default:
            return "The way to Merdeka is open. Run.";
    }
}

}