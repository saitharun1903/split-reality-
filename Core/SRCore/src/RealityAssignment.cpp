#include "srcore/RealityAssignment.h"

namespace sr {

const PlayerSlot* RealityAssignment::Find(PlayerId player) const {
    for (const PlayerSlot& s : slots_) {
        if (s.player == player) return &s;
    }
    return nullptr;
}

bool RealityAssignment::HomeTaken(Reality r) const {
    for (const PlayerSlot& s : slots_) {
        if (s.home == r) return true;
    }
    return false;
}

JoinResult RealityAssignment::AddWithHome(PlayerId player, Reality home) {
    PlayerSlot slot;
    slot.player = player;
    slot.home = home;
    slots_.push_back(slot);
    Recompute();
    return JoinResult::Ok;
}

JoinResult RealityAssignment::Join(PlayerId player) {
    if (player == kInvalidPlayer) return JoinResult::InvalidPlayer;
    if (Find(player) != nullptr) return JoinResult::AlreadyJoined;
    if (PlayerCount() >= kMaxPlayers) return JoinResult::SessionFull;
    for (Reality r : kAllRealities) {
        if (!HomeTaken(r)) return AddWithHome(player, r);
    }
    return JoinResult::SessionFull;  // unreachable while kMaxPlayers == kRealityCount
}

JoinResult RealityAssignment::Rejoin(PlayerId player, Reality previousHome) {
    if (player == kInvalidPlayer) return JoinResult::InvalidPlayer;
    if (Find(player) != nullptr) return JoinResult::AlreadyJoined;
    if (PlayerCount() >= kMaxPlayers) return JoinResult::SessionFull;
    if (!HomeTaken(previousHome)) return AddWithHome(player, previousHome);
    return Join(player);
}

bool RealityAssignment::Leave(PlayerId player) {
    for (size_t i = 0; i < slots_.size(); ++i) {
        if (slots_[i].player == player) {
            slots_.erase(slots_.begin() + static_cast<long>(i));
            Recompute();
            return true;
        }
    }
    return false;
}

PlayerId RealityAssignment::CoveringPlayer(Reality r) const {
    for (const PlayerSlot& s : slots_) {
        if (s.covered.Contains(r)) return s.player;
    }
    return kInvalidPlayer;
}

void RealityAssignment::Recompute() {
    for (PlayerSlot& s : slots_) s.covered = RealityMask::Of(s.home);
    if (slots_.empty()) return;

    // Pass 1: orphaned realities whose partner has a home holder.
    for (Reality r : kAllRealities) {
        if (HomeTaken(r)) continue;
        for (PlayerSlot& s : slots_) {
            if (s.home == Partner(r)) {
                s.covered = s.covered.With(r);
                break;
            }
        }
    }
    // Pass 2: anything still uncovered goes to the least-loaded player
    // (earliest joiner wins ties because slots_ is in join order).
    for (Reality r : kAllRealities) {
        bool covered = false;
        for (const PlayerSlot& s : slots_) covered = covered || s.covered.Contains(r);
        if (covered) continue;
        PlayerSlot* best = &slots_[0];
        for (PlayerSlot& s : slots_) {
            if (s.covered.Count() < best->covered.Count()) best = &s;
        }
        best->covered = best->covered.With(r);
    }
}

}  // namespace sr
