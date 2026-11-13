#include "srcore/CoopPuzzle.h"

#include <algorithm>
#include <utility>

namespace sr {

const char* ToString(SubmitResult r) {
    switch (r) {
        case SubmitResult::Accepted: return "Accepted";
        case SubmitResult::Completed: return "Completed";
        case SubmitResult::Mistake: return "Mistake";
        case SubmitResult::MistakeLockout: return "MistakeLockout";
        case SubmitResult::WrongReality: return "WrongReality";
        case SubmitResult::UnknownInput: return "UnknownInput";
        case SubmitResult::NotActive: return "NotActive";
        case SubmitResult::LockedOut: return "LockedOut";
        case SubmitResult::Duplicate: return "Duplicate";
    }
    return "Unknown";
}

std::string Validate(const PuzzleDefinition& def) {
    if (def.id.empty()) return "puzzle id is empty";
    if (def.solution.empty()) return "puzzle '" + def.id + "' has no solution steps";
    for (size_t i = 0; i < def.inputs.size(); ++i) {
        if (def.inputs[i].id.empty()) return "input with empty id";
        for (size_t j = i + 1; j < def.inputs.size(); ++j) {
            if (def.inputs[i].id == def.inputs[j].id) {
                return "duplicate input id '" + def.inputs[i].id + "'";
            }
        }
    }
    for (const PuzzleStep& step : def.solution) {
        bool found = false;
        for (const PuzzleInput& in : def.inputs) found = found || in.id == step.inputId;
        if (!found) return "solution references unknown input '" + step.inputId + "'";
    }
    if (def.maxMistakesBeforeLockout < 0) return "maxMistakesBeforeLockout is negative";
    if (def.lockoutSeconds < 0.0f) return "lockoutSeconds is negative";
    return std::string();
}

PuzzleInstance::PuzzleInstance(PuzzleDefinition def) : def_(std::move(def)) {}

void PuzzleInstance::Activate() {
    if (state_ == PuzzleState::Inactive) state_ = PuzzleState::Active;
}

void PuzzleInstance::Reset() {
    state_ = PuzzleState::Active;
    progress_ = 0;
    mistakes_ = 0;
    lockout_ = 0.0f;
    // Request ids are deliberately kept so old in-flight RPCs stay rejected.
}

const PuzzleInput* PuzzleInstance::FindInput(const std::string& id) const {
    for (const PuzzleInput& in : def_.inputs) {
        if (in.id == id) return &in;
    }
    return nullptr;
}

SubmitResult PuzzleInstance::RegisterMistake() {
    progress_ = 0;
    ++mistakes_;
    if (def_.maxMistakesBeforeLockout > 0 && mistakes_ >= def_.maxMistakesBeforeLockout) {
        state_ = PuzzleState::LockedOut;
        lockout_ = def_.lockoutSeconds;
        mistakes_ = 0;
        return SubmitResult::MistakeLockout;
    }
    return SubmitResult::Mistake;
}

SubmitResult PuzzleInstance::Submit(const PuzzleActor& actor, uint32_t requestId,
                                    const std::string& inputId, int32_t value) {
    // 1. Replay protection, before anything else can change state.
    auto it = std::find_if(lastRequests_.begin(), lastRequests_.end(),
                           [&](const LastRequest& r) { return r.player == actor.player; });
    if (it != lastRequests_.end()) {
        if (requestId <= it->requestId) return SubmitResult::Duplicate;
        it->requestId = requestId;
    } else {
        lastRequests_.push_back({actor.player, requestId});
    }

    // 2. State gates.
    if (state_ == PuzzleState::Inactive || state_ == PuzzleState::Solved) {
        return SubmitResult::NotActive;
    }
    if (state_ == PuzzleState::LockedOut) return SubmitResult::LockedOut;

    // 3. The actor must be able to perceive the input. Not a mistake: the
    //    game layer should never have offered the interaction, so this is
    //    either a bug or a tampered client.
    const PuzzleInput* input = FindInput(inputId);
    if (input == nullptr) return SubmitResult::UnknownInput;
    if (actor.perceiving != input->operableFrom) return SubmitResult::WrongReality;

    // 4. Judge against the expected step.
    const PuzzleStep& expected = def_.solution[static_cast<size_t>(progress_)];
    const bool conditionOk =
        expected.requiredCondition.empty() || Condition(expected.requiredCondition);
    if (expected.inputId != inputId || expected.value != value || !conditionOk) {
        return RegisterMistake();
    }

    ++progress_;
    if (progress_ >= StepCount()) {
        state_ = PuzzleState::Solved;
        return SubmitResult::Completed;
    }
    return SubmitResult::Accepted;
}

void PuzzleInstance::SetCondition(const std::string& name, bool value) {
    auto it = std::find(activeConditions_.begin(), activeConditions_.end(), name);
    if (value && it == activeConditions_.end()) activeConditions_.push_back(name);
    if (!value && it != activeConditions_.end()) activeConditions_.erase(it);
}

bool PuzzleInstance::Condition(const std::string& name) const {
    return std::find(activeConditions_.begin(), activeConditions_.end(), name) !=
           activeConditions_.end();
}

void PuzzleInstance::Tick(float deltaSeconds) {
    if (state_ != PuzzleState::LockedOut || deltaSeconds <= 0.0f) return;
    lockout_ -= deltaSeconds;
    if (lockout_ <= 0.0f) {
        lockout_ = 0.0f;
        state_ = PuzzleState::Active;
        progress_ = 0;
    }
}

}  // namespace sr
