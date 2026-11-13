// SPLIT REALITY — SRCore
// "Phasing": a player temporarily perceiving a reality other than their home.
// The server owns one PhaseState per player; clients may run a copy for
// prediction but the server's result is authoritative.
#pragma once

#include <cstdint>

#include "srcore/Reality.h"

namespace sr {

struct PhaseConfig {
    float maxEnergy = 100.0f;
    float activationCost = 15.0f;     // paid when leaving home
    float drainPerSecond = 8.0f;      // while away from home
    float regenPerSecond = 12.0f;     // while at home
    float cooldownSeconds = 3.0f;     // after any successful phase
    float minEnergyToPhase = 20.0f;   // must have at least this to leave home
};

enum class PhaseResult : uint8_t {
    Ok,
    AlreadyThere,
    NotPermitted,        // target not in the player's allowed mask
    OnCooldown,
    InsufficientEnergy,
};

const char* ToString(PhaseResult r);

class PhaseState {
public:
    PhaseState(Reality home, const PhaseConfig& config);

    // Validate without changing state (used for UI and client prediction).
    PhaseResult CanPhase(Reality target, RealityMask allowed) const;

    // Authoritative request. `allowed` = realities the player covers plus any
    // temporary grants from the mission. Returning home is always permitted
    // (subject to cooldown) and costs nothing.
    PhaseResult RequestPhase(Reality target, RealityMask allowed);

    // Advance time. If energy runs out while away, the player is pulled home
    // and ForcedReturnCount() increments (the game layer plays the snap-back).
    void Tick(float deltaSeconds);

    // Called when the assignment changes the player's home (rare: rejoin).
    void SetHome(Reality home);

    Reality Home() const { return home_; }
    Reality Current() const { return current_; }
    bool IsAway() const { return current_ != home_; }
    float Energy() const { return energy_; }
    float CooldownRemaining() const { return cooldown_; }
    uint32_t ForcedReturnCount() const { return forcedReturns_; }

private:
    PhaseConfig config_;
    Reality home_;
    Reality current_;
    float energy_;
    float cooldown_ = 0.0f;
    uint32_t forcedReturns_ = 0;
};

}  // namespace sr
