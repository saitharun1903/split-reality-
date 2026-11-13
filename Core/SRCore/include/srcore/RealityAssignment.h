// SPLIT REALITY — SRCore
// Server-side bookkeeping of which player anchors which reality.
#pragma once

#include <cstdint>
#include <vector>

#include "srcore/Reality.h"

namespace sr {

using PlayerId = uint32_t;
constexpr PlayerId kInvalidPlayer = 0;
constexpr int kMaxPlayers = 4;

struct PlayerSlot {
    PlayerId player = kInvalidPlayer;
    Reality home = Reality::Past;  // the reality this player is anchored to
    RealityMask covered;           // realities this player is responsible for
};

enum class JoinResult : uint8_t { Ok, AlreadyJoined, SessionFull, InvalidPlayer };

// Assigns home realities and covered realities.
//
// Rules (deterministic, so every reconnect or late join gives the same answer):
//  * A joining player takes the first reality, in the order Past, Present,
//    Future, Echo, that nobody holds as home.
//  * Players keep their home reality when others join or leave.
//  * Every reality is always covered by exactly one player:
//      its home holder, else the home holder of its Partner(),
//      else the player covering the fewest realities (ties: earliest joiner).
//  * A player who reconnects with the same PlayerId gets their previous home
//    back if it is still free (see Rejoin).
class RealityAssignment {
public:
    JoinResult Join(PlayerId player);
    bool Leave(PlayerId player);

    // Rejoin after a disconnect: prefers the player's previous home reality.
    JoinResult Rejoin(PlayerId player, Reality previousHome);

    int PlayerCount() const { return static_cast<int>(slots_.size()); }
    const std::vector<PlayerSlot>& Slots() const { return slots_; }

    // Returns nullptr if the player is not in the session.
    const PlayerSlot* Find(PlayerId player) const;

    // The player responsible for a reality, or kInvalidPlayer if the session is empty.
    PlayerId CoveringPlayer(Reality r) const;

private:
    JoinResult AddWithHome(PlayerId player, Reality home);
    bool HomeTaken(Reality r) const;
    void Recompute();

    std::vector<PlayerSlot> slots_;  // in join order
};

}  // namespace sr
