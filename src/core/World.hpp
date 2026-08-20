#pragma once
#include "MapGenerator.hpp"
#include "Rng.hpp"
#include "Map.hpp"
#include "Types.hpp"
#include "Player.hpp"
#include "Monster.hpp"
#include <vector>
#include <string>
#include <memory>
#include <deque>

class World {
public:
    World(GeneratedMap generated, Rng& rng);

    // Title -> Play
    void begin();

    void playerMove(const Vec2i& delta);

    const Map& map() const;

    const Player& player() const;
    Player& player();

    const std::vector<std::unique_ptr<Monster>>& monsters() const;

    const std::vector<Vec2i>& tokenPositions() const;

    const std::deque<std::string>& messages() const;

    Monster* monsterAt(const Vec2i& p) const;

    bool isFree(const Vec2i& p) const;
    GameState state() const;
    int alarm() const;

    // What the player currently knows about a tile
    Visibility visibilityAt(const Vec2i& p) const;

    struct MoveEvent {
        EntityKind kind;
        Vec2i from;
        Vec2i to;
    };

    const std::vector<MoveEvent>& lastTurnMoves() const;

    void moveEntity(Entity& entity, const Vec2i& to);

    void attack(Entity& attacker, Entity& defender);
    void log(std::string message);

    void debugGrantToken();
    void debugKillMonsters();
    void debugHurtPlayer(int amount);
    void debugRevealMap();

private:
    void advanceTurn();
    void checkPlayerDeath();
    void refreshVisibility();

    // What standing on a tile means a token to lift, or the Hall to escape by.
    void onPlayerEntered(const Vec2i& p);
    void takeTokenAt(int index);
    void raiseAlarm();
    void spawnHunters(int count);
    std::vector<Vec2i> freeSpawnTiles(int minDistanceFromPlayer) const;

    Map m_map;
    Player m_player;
    std::vector<std::unique_ptr<Monster>> m_monster;
    std::vector<Vec2i> m_tokenPositions;
    std::deque<std::string> m_message;
    std::vector<MoveEvent> m_lastTurnMoves;
    // One Visibility per tile
    std::vector<Visibility> m_visibility;
    Rng& m_rng;
    GameState m_state = GameState::Title;
    int m_alarm = 0;
};
