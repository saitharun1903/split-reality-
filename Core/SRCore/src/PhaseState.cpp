#include "srcore/PhaseState.h"

namespace sr {

const char* ToString(PhaseResult r) {
    switch (r) {
        case PhaseResult::Ok: return "Ok";
        case PhaseResult::AlreadyThere: return "AlreadyThere";
        case PhaseResult::NotPermitted: return "NotPermitted";
        case PhaseResult::OnCooldown: return "OnCooldown";
        case PhaseResult::InsufficientEnergy: return "InsufficientEnergy";
    }
    return "Unknown";
}

PhaseState::PhaseState(Reality home, const PhaseConfig& config)
    : config_(config), home_(home), current_(home), energy_(config.maxEnergy) {}

PhaseResult PhaseState::CanPhase(Reality target, RealityMask allowed) const {
    if (target == current_) return PhaseResult::AlreadyThere;
    if (cooldown_ > 0.0f) return PhaseResult::OnCooldown;
    if (target == home_) return PhaseResult::Ok;  // returning home is always allowed
    if (!allowed.Contains(target)) return PhaseResult::NotPermitted;
    // Hopping between two away realities costs activation again but does not
    // require the minimum threshold; leaving home does.
    if (current_ == home_ && energy_ < config_.minEnergyToPhase) {
        return PhaseResult::InsufficientEnergy;
    }
    if (energy_ < config_.activationCost) return PhaseResult::InsufficientEnergy;
    return PhaseResult::Ok;
}

PhaseResult PhaseState::RequestPhase(Reality target, RealityMask allowed) {
    const PhaseResult result = CanPhase(target, allowed);
    if (result != PhaseResult::Ok) return result;
    if (target != home_) energy_ -= config_.activationCost;
    current_ = target;
    cooldown_ = config_.cooldownSeconds;
    return PhaseResult::Ok;
}

void PhaseState::Tick(float deltaSeconds) {
    if (deltaSeconds <= 0.0f) return;
    cooldown_ = cooldown_ > deltaSeconds ? cooldown_ - deltaSeconds : 0.0f;

    if (IsAway()) {
        energy_ -= config_.drainPerSecond * deltaSeconds;
        if (energy_ <= 0.0f) {
            energy_ = 0.0f;
            current_ = home_;
            cooldown_ = config_.cooldownSeconds;
            ++forcedReturns_;
        }
    } else {
        energy_ += config_.regenPerSecond * deltaSeconds;
        if (energy_ > config_.maxEnergy) energy_ = config_.maxEnergy;
    }
}

void PhaseState::SetHome(Reality home) {
    home_ = home;
    current_ = home;
    cooldown_ = 0.0f;
}

}  // namespace sr
