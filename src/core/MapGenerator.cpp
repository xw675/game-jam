#include "MapGenerator.hpp"
#include "Connectivity.hpp"
#include "Balance.hpp"
#include <algorithm>
#include <cassert>

static_assert(Balance::kMinRooms >= Balance::kTokenCount + 2, 
    "need one room for the player, kTokenCount for tokens, and one for the Hall");

namespace {

constexpr int kRoomMinSize = 4;
constexpr int kRoomMaxW = 8;
constexpr int kRoomMaxH = 6;
constexpr int kRoomPadding = 1; // wall tiles guaranteed between any two rooms
constexpr int kMaxMapAttempts = 100;

struct Room {
    int x = 0, y = 0, w = 0, h = 0;

    Vec2i center() const {
        return {x + w / 2, y + h / 2};
    }

    // kRoomPadding widens each room by a tile for the test only, so rooms that
    // would share a wall are rejected and there is always rock between them.
    bool tooCloseTo(const Room& o) const {
        const bool clearOnX = x >= o.x + o.w + kRoomPadding || o.x >= x + w + kRoomPadding;
        const bool clearOnY = y >= o.y + o.h + kRoomPadding || o.y >= y + h + kRoomPadding;
        return !(clearOnX || clearOnY);
    }
};

void carveRoom(Map& map, const Room& r) {
    for (int y = r.y; y < r.y + r.h; ++y) {
        for (int x = r.x; x < r.x + r.w; ++x) {
            map.set({x, y}, Tile::Floor);
        }
    }    
}

// L-shaped: horizontal along a.y, then vertical along b.x.
// Carving only ever turns rock into floor, so a corridor that happens to cross
// a room already carved does no harm.
void carveCorridor(Map& map, const Vec2i& a, const Vec2i& b) {
    for (int x = std::min(a.x, b.x); x <= std::max(a.x, b.x); ++x) map.set({x, a.y}, Tile::Floor);
    for (int y = std::min(a.y, b.y); y <= std::max(a.y, b.y); ++y) map.set({b.x, y}, Tile::Floor);
}

std::vector<Room> placeRooms(Rng& rng, int width, int height, int roomAttempts) {
    std::vector<Room> rooms;
    for (int i = 0; i < roomAttempts; ++i) {
        Room r;
        r.w = rng.range(kRoomMinSize, kRoomMaxW);
        r.h = rng.range(kRoomMinSize, kRoomMaxH);
        // The 1 keep a solid border around the map so no room touches the edge.
        r.x = rng.range(1, width - r.w - 1);
        r.y = rng.range(1, height - r.h - 1);

        const bool clash = std::any_of(rooms.begin(), rooms.end(),[&](const Room& other) { return r.tooCloseTo(other); });
        if (!clash) {
            rooms.push_back(r);
        }
    }
    return rooms;
}

std::size_t farthestRoomFrom(const std::vector<Room>& rooms, const Vec2i& from) {
    std::size_t best = 0;
    int bestDist = -1;
    for (std::size_t i = 0; i < rooms.size(); ++i) {
        const int d = manhattan(rooms[i].center(), from);
        if (d > bestDist) { bestDist = d; best = i; }
    }
    return best;
}
}

GeneratedMap generateMap(Rng& rng, int width, int height, int roomAttempts) {
    // precondition
    assert(width  >= kRoomMaxW + 3 && "map too narrow for a room plus its border");
    assert(height >= kRoomMaxH + 3 && "map too short for a room plus its border");
    assert(roomAttempts >= Balance::kMinRooms);

    // Generate and check: build a whole candidate dungeon, 
    // if it is unacceptable, discard it and build another.
    for (int attempt = 0; attempt < kMaxMapAttempts; ++attempt) {
        const std::vector<Room> rooms = placeRooms(rng, width, height, roomAttempts);
        if (static_cast<int>(rooms.size()) < Balance::kMinRooms) continue;

        // Map's constructor fills every tile with Tile::Wall, so generation is
        // subtractive, the dungeon is carved out of solid rock.
        GeneratedMap gen{Map(width, height), {}, {}, {}};

        // Every room is joined to the one before it, so the rooms form a single unbroken chain.
        for (std::size_t i = 0; i < rooms.size(); ++i) {
            carveRoom(gen.map, rooms[i]);
            if (i > 0) carveCorridor(gen.map, rooms[i - 1].center(), rooms[i].center());
        }

        gen.playerStart = rooms.front().center();

        // The Hall goes as far from the spawn as possible. Picking the last room placed 
        // instead put it within 15 tiles of the player on roughly a quarter of maps.
        const std::size_t hallRoom = farthestRoomFrom(rooms, gen.playerStart);
        assert(hallRoom != 0 && "the Hall must not be the player's own room");
        gen.hall = rooms[hallRoom].center();
        gen.map.set(gen.hall, Tile::Hall); // still walkable, only Wall blocks

        // Tokens fill the earliest remaining rooms, skipping the player's and the Hall's
        for (std::size_t i = 1;
             i < rooms.size() && static_cast<int>(gen.tokenPositions.size()) < Balance::kTokenCount;
             ++i) {
            if (i == hallRoom) continue;
            gen.tokenPositions.push_back(rooms[i].center());
        }
        assert(static_cast<int>(gen.tokenPositions.size()) == Balance::kTokenCount);

        // confirm the player can actually walk to all four tokens and the Hall.
        std::vector<Vec2i> targets = gen.tokenPositions;
        targets.push_back(gen.hall);
        if (allReachable(gen.map, gen.playerStart, targets)) return gen;
    }

    assert(false && "generateMap: no connected layout within kMaxMapAttempts");
    return {Map(width, height), {}, {}, {}};
}
