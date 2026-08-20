#include "World.hpp"
#include "Hunter.hpp"
#include "Balance.hpp"
#include "Dice.hpp"
#include "Fov.hpp"
#include <algorithm>
#include <cstddef>
#include <utility>

namespace {

constexpr std::size_t kMaxMessages = 3;

const char* kindName(EntityKind kind) {
    switch (kind) {
        case EntityKind::Player: 
            return "You";
        case EntityKind::Hunter:
            return "The hunter";
        case EntityKind::Token:
            return "The token";
        case EntityKind::Hall:
            return "The hall";
    }
    return "Something";
}

}

World::World(GeneratedMap generated, Rng& rng)
    : m_map(std::move(generated.map)),
      m_player(generated.playerStart),
      m_tokenPositions(std::move(generated.tokenPositions)),
      m_rng(rng) {
    for (const Vec2i& guardPost : m_tokenPositions) {
        m_monster.push_back(std::make_unique<Hunter>(guardPost));
    }
    refreshVisibility(); // the player can see their own starting room
    log("Steal the four tokens, then reach the Hall.");
}

void World::begin() {
    if (m_state == GameState::Title) {
        m_state = GameState::Play;
    }
}

void World::refreshVisibility() {
    updateVisibility(m_map, m_player.position(), Balance::kVisionRadius, m_visibility);
}

Visibility World::visibilityAt(const Vec2i& p) const {
    if (!m_map.inBounds(p)) {
        return Visibility::Unseen;
    }
    return m_visibility[static_cast<std::size_t>(p.y * m_map.width() + p.x)];
}

const Map& World::map() const {
    return m_map;
}

const Player& World::player() const {
    return m_player;
}

Player& World::player() {
    return m_player;
}

const std::vector<std::unique_ptr<Monster>>& World::monsters() const {
    return m_monster;
}

const std::vector<Vec2i>& World::tokenPositions() const {
    return m_tokenPositions;
}

const std::deque<std::string>& World::messages() const {
    return m_message;
}

const std::vector<World::MoveEvent>& World::lastTurnMoves() const {
    return m_lastTurnMoves;
}

GameState World::state() const {
    return m_state;
}

int World::alarm() const {
    return m_alarm;
}

Monster* World::monsterAt(const Vec2i& p) const {
    for (const std::unique_ptr<Monster>& monster : m_monster) {
        if (monster->isAlive() && monster->position() == p) {
            return monster.get();
        }
    }
    return nullptr;
}

bool World::isFree(const Vec2i& p) const {
    return m_map.isWalkable(p) && monsterAt(p) == nullptr && !(p == m_player.position());
}

void World::moveEntity(Entity& entity, const Vec2i& to) {
    m_lastTurnMoves.push_back({entity.kind(), entity.position(), to});
    entity.moveTo(to);
}

void World::playerMove(const Vec2i& delta) {
    if (m_state != GameState::Play) {
        return;
    }
    if (delta.x == 0 && delta.y == 0) {
        return;
    }

    m_lastTurnMoves.clear();

    const Vec2i target = m_player.position() + delta;
    if (!m_map.isWalkable(target)) {
        return;
    }

    if (Monster* monster = monsterAt(target)) {
        attack(m_player, *monster);
    } else {
        moveEntity(m_player, target);
        onPlayerEntered(target);
    }

    // Recomputed once per accepted input, not once per frame.
    refreshVisibility();

    // Reaching the Hall ends the run on the spot
    if (m_state == GameState::Play) {
        advanceTurn();
    }
}

void World::onPlayerEntered(const Vec2i& p) {
    for (std::size_t i = 0; i < m_tokenPositions.size(); ++i) {
        const int index = static_cast<int>(i);
        if (m_tokenPositions[i] == p && !m_player.hasToken(index)) {
            takeTokenAt(index);
            return; // a tile holds at most one token, so stop looking
        }
    }

    if (m_map.at(p) == Tile::Hall) {
        if (m_player.hasAllTokens()) {
            m_state = GameState::Won;
            log("You are out, and all four are yours. Press R to play again.");
        } else {
            log("The Hall is shut. It opens to four.");
        }
    }
}

void World::takeTokenAt(int index) {
    m_player.takeToken(index);

    m_player.heal(Balance::kTokenHeal);

    log("You lift the token and it makes you whole. The keep hears it.");
    raiseAlarm();
}

void World::raiseAlarm() {
    ++m_alarm;
    // Flat per token, not alarm x count.
    spawnHunters(Balance::kHuntersPerAlarm);
}

void World::spawnHunters(int count) {
    // Built once, then drawn from without replacement, so two hunters can never land on the same tile.
    std::vector<Vec2i> candidates = freeSpawnTiles(Balance::kSpawnMinDistance);

    for (int i = 0; i < count && !candidates.empty(); ++i) {
        const int pick = m_rng.range(0, static_cast<int>(candidates.size()) - 1);
        m_monster.push_back(std::make_unique<Hunter>(candidates[static_cast<std::size_t>(pick)]));
        candidates.erase(candidates.begin() + static_cast<std::ptrdiff_t>(pick));
    }
}

std::vector<Vec2i> World::freeSpawnTiles(int minDistanceFromPlayer) const {
    std::vector<Vec2i> tiles;
    for (int y = 0; y < m_map.height(); ++y) {
        for (int x = 0; x < m_map.width(); ++x) {
            const Vec2i p{x, y};
            if (isFree(p) && manhattan(p, m_player.position()) >= minDistanceFromPlayer) {
                tiles.push_back(p);
            }
        }
    }
    return tiles;
}

void World::attack(Entity& attacker, Entity& defender) {
    const AttackResult result =
        resolveAttack(m_rng, attacker.might(), defender.defence(), attacker.damageDie());

    defender.takeDamage(result.damage);

    const bool byPlayer = attacker.kind() == EntityKind::Player;
    const std::string who = kindName(attacker.kind());
    const std::string whom = kindName(defender.kind());
    const std::string hurt = " for " + std::to_string(result.damage);
    const std::string maths = " (d20: " + std::to_string(result.roll) 
                            + " +" + std::to_string(attacker.might())
                            + " = " + std::to_string(result.total)
                            + " vs " + std::to_string(defender.defence()) + ")";

    switch (result.outcome) {
        case AttackOutcome::Miss:
            log(who + (byPlayer ? " miss " : " misses ") + whom + maths);
            break;
        case AttackOutcome::Graze:
            log(who + (byPlayer ? " graze " : " grazes ") + whom + hurt + maths);
            break;
        case AttackOutcome::Hit:
            log(who + (byPlayer ? " hit " : " hits ") + whom + hurt + maths);
            break;
        case AttackOutcome::Critical:
            log(who + (byPlayer ? " crit " : " crits ") + whom + hurt + maths);
            break;
    }

    if (!defender.isAlive()) {
        log(defender.kind() == EntityKind::Player ? std::string("You are cut down.") 
        : std::string(kindName(defender.kind())) + " falls.");
    }
}

void World::log(std::string message) {
    m_message.push_back(std::move(message));
    while (m_message.size() > kMaxMessages) {
        m_message.pop_front();
    }
}

void World::advanceTurn() {
    for (const std::unique_ptr<Monster>& monster : m_monster) {
        if (monster->isAlive()) {
            monster->takeTurn(*this);
        }
    }

    m_monster.erase(
        std::remove_if(m_monster.begin(), m_monster.end(),
                       [](const std::unique_ptr<Monster>& m) { 
                        return !m->isAlive(); 
                    }),
        m_monster.end());

    checkPlayerDeath();
}

void World::checkPlayerDeath() {
    if (!m_player.isAlive()) {
        m_state = GameState::Dead;
        log("You die. Press R to begin again.");
    }
}

void World::debugGrantToken() {
    for (std::size_t i = 0; i < m_tokenPositions.size(); ++i) {
        if (!m_player.hasToken(static_cast<int>(i))) {
            takeTokenAt(static_cast<int>(i)); // same path as real pickup
            return;
        }
    }
}

void World::debugKillMonsters() {
    m_monster.clear();
}

void World::debugHurtPlayer(int amount) {
    m_player.takeDamage(amount);
    checkPlayerDeath();
}

// Marking every tile Remembered. "seen once, never Unseen again" is already the rule, 
// so the next refreshVisibility() lights up whatever is genuinely in view and leaves the rest drawn dim.
void World::debugRevealMap() {
    std::fill(m_visibility.begin(), m_visibility.end(), Visibility::Remembered);
    refreshVisibility();
}
