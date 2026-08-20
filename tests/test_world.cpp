#include <catch_amalgamated.hpp>

#include "SequenceRng.hpp"
#include "core/Balance.hpp"
#include "core/MapGenerator.hpp"
#include "core/Types.hpp"
#include "core/World.hpp"

#include <utility>
#include <vector>

namespace {

GeneratedMap makeCorridor() {
    Map map(30, 3);
    for (int x = 1; x < 29; ++x) {
        map.set({x, 1}, Tile::Floor);
    }

    GeneratedMap gen{std::move(map), {1, 1}, {{20, 1}, {22, 1}, {24, 1}, {26, 1}}, {3, 1}};
    gen.map.set(gen.hall, Tile::Hall);
    return gen;
}

}

TEST_CASE("the Hall opens only to four tokens, and each token raises the alarm once",
          "[world]") {
    // World spawns one hunter per token and one more per alarm, and each spawn
    // costs exactly one roll. Four 1s pick the first candidate tile every time.
    SequenceRng rng{{1, 1, 1, 1}};
    World world(makeCorridor(), rng);
    world.begin(); // the run starts in GameState::Title; nothing moves until then

    world.debugKillMonsters();
    REQUIRE(world.alarm() == 0);

    SECTION("three tokens is not enough") {
        for (int i = 0; i < 3; ++i) {
            world.debugGrantToken();
            world.debugKillMonsters(); // each alarm spawns a fresh hunter
        }
        REQUIRE(world.alarm() == 3);
        REQUIRE_FALSE(world.player().hasAllTokens());

        world.playerMove({1, 0}); // to x=2
        world.playerMove({1, 0}); // onto the Hall at x=3

        REQUIRE(world.player().position() == Vec2i{3, 1});
        REQUIRE(world.state() == GameState::Play); // shut

        SECTION("and the fourth opens it") {
            world.debugGrantToken();
            world.debugKillMonsters();

            REQUIRE(world.alarm() == Balance::kTokenCount);
            REQUIRE(world.player().hasAllTokens());

            world.playerMove({-1, 0});
            world.playerMove({1, 0});

            REQUIRE(world.state() == GameState::Won);
        }
    }

    SECTION("a token already taken cannot raise the alarm a second time") {
        for (int i = 0; i < Balance::kTokenCount; ++i) {
            world.debugGrantToken();
            world.debugKillMonsters();
        }
        REQUIRE(world.alarm() == Balance::kTokenCount);

        world.debugGrantToken();
        REQUIRE(world.alarm() == Balance::kTokenCount);
    }
}
