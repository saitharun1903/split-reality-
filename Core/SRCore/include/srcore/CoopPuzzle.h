// SPLIT REALITY — SRCore
// Reusable, data-driven cooperative puzzle. Content defines inputs (each
// operable only from one reality), an ordered solution and named conditions
// (flags raised by other systems, e.g. "a surge window is open"). The server
// owns the PuzzleInstance and is the only place submissions are judged.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "srcore/Reality.h"
#include "srcore/RealityAssignment.h"

namespace sr {

struct PuzzleInput {
    std::string id;
    Reality operableFrom = Reality::Present;
};

struct PuzzleStep {
    std::string inputId;
    int32_t value = 0;
    std::string requiredCondition;  // empty = none
};

struct PuzzleDefinition {
    std::string id;
    std::vector<PuzzleInput> inputs;
    std::vector<PuzzleStep> solution;
    int maxMistakesBeforeLockout = 3;  // 0 = never lock out
    float lockoutSeconds = 20.0f;
};

// Who is acting, as known by the server (never trusted from the client).
struct PuzzleActor {
    PlayerId player = kInvalidPlayer;
    Reality perceiving = Reality::Present;  // PhaseState::Current()
};

enum class PuzzleState : uint8_t { Inactive, Active, LockedOut, Solved };

enum class SubmitResult : uint8_t {
    Accepted,          // correct step, more to go
    Completed,         // final step, puzzle solved
    Mistake,           // wrong value or condition not met; sequence reset
    MistakeLockout,    // mistake that triggered lockout
    WrongReality,      // actor cannot perceive this input; not a mistake
    UnknownInput,
    NotActive,
    LockedOut,
    Duplicate,         // stale or repeated request id from this player
};

const char* ToString(SubmitResult r);

// Returns an empty string if valid, otherwise a description of the first problem.
std::string Validate(const PuzzleDefinition& def);

class PuzzleInstance {
public:
    explicit PuzzleInstance(PuzzleDefinition def);

    void Activate();
    void Reset();  // back to Active with no progress (checkpoint restore)

    // requestId must increase per player; repeats are rejected as Duplicate so
    // retransmitted RPCs can never advance or fail the puzzle twice.
    SubmitResult Submit(const PuzzleActor& actor, uint32_t requestId,
                        const std::string& inputId, int32_t value);

    void SetCondition(const std::string& name, bool value);
    bool Condition(const std::string& name) const;

    void Tick(float deltaSeconds);

    PuzzleState State() const { return state_; }
    int Progress() const { return progress_; }
    int StepCount() const { return static_cast<int>(def_.solution.size()); }
    int Mistakes() const { return mistakes_; }
    float LockoutRemaining() const { return lockout_; }
    const PuzzleDefinition& Definition() const { return def_; }

private:
    const PuzzleInput* FindInput(const std::string& id) const;
    SubmitResult RegisterMistake();

    struct LastRequest {
        PlayerId player;
        uint32_t requestId;
    };

    PuzzleDefinition def_;
    PuzzleState state_ = PuzzleState::Inactive;
    int progress_ = 0;
    int mistakes_ = 0;
    float lockout_ = 0.0f;
    std::vector<std::string> activeConditions_;
    std::vector<LastRequest> lastRequests_;
};

}  // namespace sr
