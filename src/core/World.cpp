#include "World.hpp"
#include "Hunter.hpp"
#include <algorithm>
#include <utility>

namespace {

constexpr std::size_t kMaxMessages = 3;

const char* kindName(EntityKind kind) {
    switch (kind) {
        case EntityKind::Player: return "You";
        case EntityKind::Wanderer: return "The wanderer";
        case EntityKind::Hunter: return "The hunter";
        case EntityKind::Guard: return "The guard";
        case EntityKind::Token: return "The token";
        case EntityKind::Hall: return "The hall";
    }
    return "Something";
}

}

World::World(GeneratedMap generated, Rng& rng)
    : m_map(std::move(generated.map)),
      m_player(generated.playerStart),
      m_tokens(std::move(generated.tokenPositions)),
      m_rng(rng) {
    for (const Vec2i& guardPost : m_tokens) {
        m_monster.push_back(std::make_unique<Hunter>(guardPost));
    }
    log("Steal the four tokens, then reach the Hall.");
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

const std::vector<Vec2i>& World::tokens() const {
    return m_tokens;
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
    }

    advanceTurn();
}

void World::attack(Entity& attacker, Entity& defender) {
    const int roll = m_rng.roll(20);
    const int total = roll + attacker.might();

    const bool byPlayer = attacker.kind() == EntityKind::Player;
    const std::string margin = " (" + std::to_string(total)
                             + " vs " + std::to_string(defender.defence()) + ").";

    if (total < defender.defence()) {
        log(std::string(kindName(attacker.kind())) + (byPlayer ? " miss " : " misses ")
            + kindName(defender.kind()) + margin);
        return;
    }

    const int damage = m_rng.roll(attacker.damageDie());
    defender.takeDamage(damage);

    log(std::string(kindName(attacker.kind())) + (byPlayer ? " hit " : " hits ")
        + kindName(defender.kind()) + " for " + std::to_string(damage) + margin);

    if (!defender.isAlive()) {
        log(defender.kind() == EntityKind::Player
                ? std::string("You are cut down.")
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
        std::remove_if(m_monster.begin(), m_monster.end(), [](const std::unique_ptr<Monster>& m) { return !m->isAlive(); }),
        m_monster.end());

    if (!m_player.isAlive()) {
        m_state = GameState::Dead;
        log("You die. Press R to begin again.");
    }
}
