#include <catch_amalgamated.hpp>

#include "core/Balance.hpp"
#include "core/Connectivity.hpp"
#include "core/MapGenerator.hpp"
#include "core/Rng.hpp"

#include <cstdint>
#include <vector>

TEST_CASE("every seeded dungeon is winnable", "[mapgen]") {
    constexpr int kRoomAttempts = 120; // the same number main.cpp passes
    constexpr std::uint32_t kSeeds = 200;

    for (std::uint32_t seed = 0; seed < kSeeds; ++seed) {
        DefaultRng rng(seed);
        const GeneratedMap gen =
            generateMap(rng, Balance::kMapWidth, Balance::kMapHeight, kRoomAttempts);

        INFO("seed " << seed); // Catch2 prints this only when something below fails

        REQUIRE(static_cast<int>(gen.tokenPositions.size()) == Balance::kTokenCount);
        REQUIRE(gen.map.isWalkable(gen.playerStart));
        REQUIRE(gen.map.at(gen.hall) == Tile::Hall);

        // The Hall must not be under the player's feet
        REQUIRE_FALSE(gen.hall == gen.playerStart);

        std::vector<Vec2i> targets = gen.tokenPositions;
        targets.push_back(gen.hall);
        REQUIRE(allReachable(gen.map, gen.playerStart, targets));
    }
}
