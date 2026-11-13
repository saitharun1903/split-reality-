#include <set>

#include "TestHarness.h"
#include "srcore/CoopPuzzle.h"
#include "srcore/MeridianGate.h"
#include "srcore/PhaseState.h"
#include "srcore/RealityAssignment.h"

using namespace sr;

static PuzzleDefinition SimplePuzzle() {
    PuzzleDefinition d;
    d.id = "Test.Simple";
    d.inputs = {{"Dial", Reality::Present}, {"Lever", Reality::Past}};
    d.solution = {{"Dial", 3, ""}, {"Lever", 1, "Ready"}};
    d.maxMistakesBeforeLockout = 2;
    d.lockoutSeconds = 5.0f;
    return d;
}

TEST(Puzzle_ValidateCatchesBadDefinitions) {
    CHECK(Validate(SimplePuzzle()).empty());
    PuzzleDefinition d = SimplePuzzle();
    d.solution.push_back({"Missing", 0, ""});
    CHECK(!Validate(d).empty());
    d = SimplePuzzle();
    d.inputs.push_back({"Dial", Reality::Echo});
    CHECK(!Validate(d).empty());
    d = SimplePuzzle();
    d.solution.clear();
    CHECK(!Validate(d).empty());
}

TEST(Puzzle_InactiveUntilActivated) {
    PuzzleInstance p(SimplePuzzle());
    CHECK_EQ(p.Submit({1, Reality::Present}, 1, "Dial", 3), SubmitResult::NotActive);
}

TEST(Puzzle_CrossRealitySolve) {
    PuzzleInstance p(SimplePuzzle());
    p.Activate();
    CHECK_EQ(p.Submit({1, Reality::Present}, 1, "Dial", 3), SubmitResult::Accepted);
    p.SetCondition("Ready", true);
    CHECK_EQ(p.Submit({2, Reality::Past}, 1, "Lever", 1), SubmitResult::Completed);
    CHECK(p.State() == PuzzleState::Solved);
    CHECK_EQ(p.Submit({2, Reality::Past}, 2, "Lever", 1), SubmitResult::NotActive);
}

TEST(Puzzle_WrongRealityIsRejectedButNotAMistake) {
    PuzzleInstance p(SimplePuzzle());
    p.Activate();
    CHECK_EQ(p.Submit({1, Reality::Echo}, 1, "Dial", 3), SubmitResult::WrongReality);
    CHECK_EQ(p.Mistakes(), 0);
    CHECK_EQ(p.Submit({1, Reality::Present}, 2, "Nope", 3), SubmitResult::UnknownInput);
}

TEST(Puzzle_MissingConditionCountsAsMistakeAndResets) {
    PuzzleInstance p(SimplePuzzle());
    p.Activate();
    p.Submit({1, Reality::Present}, 1, "Dial", 3);
    CHECK_EQ(p.Submit({2, Reality::Past}, 1, "Lever", 1), SubmitResult::Mistake);
    CHECK_EQ(p.Progress(), 0);
    CHECK_EQ(p.Mistakes(), 1);
}

TEST(Puzzle_LockoutAndRecovery) {
    PuzzleInstance p(SimplePuzzle());
    p.Activate();
    CHECK_EQ(p.Submit({1, Reality::Present}, 1, "Dial", 9), SubmitResult::Mistake);
    CHECK_EQ(p.Submit({1, Reality::Present}, 2, "Dial", 9), SubmitResult::MistakeLockout);
    CHECK(p.State() == PuzzleState::LockedOut);
    CHECK_EQ(p.Submit({1, Reality::Present}, 3, "Dial", 3), SubmitResult::LockedOut);
    p.Tick(4.9f);
    CHECK(p.State() == PuzzleState::LockedOut);
    p.Tick(0.2f);
    CHECK(p.State() == PuzzleState::Active);
    CHECK_EQ(p.Submit({1, Reality::Present}, 4, "Dial", 3), SubmitResult::Accepted);
}

TEST(Puzzle_DuplicateAndStaleRequestsIgnored) {
    PuzzleInstance p(SimplePuzzle());
    p.Activate();
    CHECK_EQ(p.Submit({1, Reality::Present}, 5, "Dial", 3), SubmitResult::Accepted);
    // A retransmitted RPC must not count as a second (wrong) press.
    CHECK_EQ(p.Submit({1, Reality::Present}, 5, "Dial", 3), SubmitResult::Duplicate);
    CHECK_EQ(p.Submit({1, Reality::Present}, 4, "Dial", 3), SubmitResult::Duplicate);
    CHECK_EQ(p.Progress(), 1);
    CHECK_EQ(p.Mistakes(), 0);
    // Request ids are per player: player 2 starting at 1 is fine.
    p.SetCondition("Ready", true);
    CHECK_EQ(p.Submit({2, Reality::Past}, 1, "Lever", 1), SubmitResult::Completed);
}

TEST(Puzzle_SimultaneousConflictingPressesResolveDeterministically) {
    // Two players hit the same input in the same server frame; the server
    // processes RPCs in arrival order, so the outcome is defined by order.
    PuzzleInstance p(SimplePuzzle());
    p.Activate();
    CHECK_EQ(p.Submit({1, Reality::Present}, 1, "Dial", 3), SubmitResult::Accepted);
    CHECK_EQ(p.Submit({3, Reality::Present}, 1, "Dial", 3), SubmitResult::Mistake);
    CHECK_EQ(p.Progress(), 0);
}

TEST(Puzzle_ResetKeepsReplayProtection) {
    PuzzleInstance p(SimplePuzzle());
    p.Activate();
    p.Submit({1, Reality::Present}, 7, "Dial", 3);
    p.Reset();
    CHECK_EQ(p.Progress(), 0);
    CHECK_EQ(p.Submit({1, Reality::Present}, 7, "Dial", 3), SubmitResult::Duplicate);
}

TEST(MeridianGate_LayoutIsDeterministicAndWellFormed) {
    const MeridianGateLayout a = GenerateMeridianGateLayout(1234);
    const MeridianGateLayout b = GenerateMeridianGateLayout(1234);
    CHECK(a.glyphSequence == b.glyphSequence);
    CHECK(a.socketForGlyph == b.socketForGlyph);
    for (uint64_t seed = 0; seed < 500; ++seed) {
        const MeridianGateLayout l = GenerateMeridianGateLayout(seed);
        std::set<int> glyphs(l.glyphSequence.begin(), l.glyphSequence.end());
        CHECK_EQ(glyphs.size(), static_cast<size_t>(MeridianGateLayout::kSequenceLength));
        std::set<int> sockets(l.socketForGlyph.begin(), l.socketForGlyph.end());
        CHECK_EQ(sockets.size(), static_cast<size_t>(MeridianGateLayout::kGlyphCount));
        CHECK(l.surgePeriodSeconds >= 9.0f && l.surgePeriodSeconds <= 14.0f);
        CHECK(l.calmWindowSeconds >= 2.5f && l.calmWindowSeconds <= 4.0f);
        CHECK(Validate(BuildMeridianGatePuzzle(l)).empty());
    }
}

TEST(MeridianGate_SeedsProduceVariety) {
    std::set<std::string> seen;
    for (uint64_t seed = 0; seed < 200; ++seed) {
        const MeridianGateLayout l = GenerateMeridianGateLayout(seed);
        std::string key;
        for (int g : l.glyphSequence) key += static_cast<char>('0' + l.socketForGlyph[g]);
        seen.insert(key);
    }
    CHECK(seen.size() > 150);  // 360 possible socket orders
}

// --- End-to-end rule simulations of the Chapter 1 gate -----------------------

namespace {

struct SimPlayer {
    PlayerId id;
    PhaseState phase;
    uint32_t nextRequest = 1;
};

SubmitResult Press(PuzzleInstance& p, SimPlayer& pl, const char* input, int value) {
    return p.Submit({pl.id, pl.phase.Current()}, pl.nextRequest++, input, value);
}

void RunSurgeUntilCalm(PuzzleInstance& p) { p.SetCondition(kMeridianCalmCondition, true); }

}  // namespace

TEST(Sim_FourPlayersSolveMeridianGate) {
    const MeridianGateLayout layout = GenerateMeridianGateLayout(0xA57E);
    PuzzleInstance gate(BuildMeridianGatePuzzle(layout));
    gate.Activate();

    RealityAssignment a;
    for (PlayerId p = 1; p <= 4; ++p) a.Join(p);
    SimPlayer present{2, PhaseState(a.Find(2)->home, PhaseConfig{})};
    CHECK_EQ(present.phase.Current(), Reality::Present);

    // Past reads glyph order, Echo reads glyph->socket; they relay to Present.
    for (int glyph : layout.glyphSequence) {
        CHECK_EQ(Press(gate, present, kMeridianSocketInput, layout.socketForGlyph[glyph]),
                 SubmitResult::Accepted);
    }
    // Future calls "now" when the surge calms.
    RunSurgeUntilCalm(gate);
    CHECK_EQ(Press(gate, present, kMeridianEngageInput, 1), SubmitResult::Completed);
}

TEST(Sim_FourPlayersEngageDuringSurgeIsPunished) {
    const MeridianGateLayout layout = GenerateMeridianGateLayout(99);
    PuzzleInstance gate(BuildMeridianGatePuzzle(layout));
    gate.Activate();
    SimPlayer present{2, PhaseState(Reality::Present, PhaseConfig{})};
    for (int glyph : layout.glyphSequence) {
        Press(gate, present, kMeridianSocketInput, layout.socketForGlyph[glyph]);
    }
    CHECK_EQ(Press(gate, present, kMeridianEngageInput, 1), SubmitResult::Mistake);
    CHECK_EQ(gate.Progress(), 0);
}

TEST(Sim_TwoPlayerFallbackSolvesWithPhasing) {
    const MeridianGateLayout layout = GenerateMeridianGateLayout(42);
    PuzzleInstance gate(BuildMeridianGatePuzzle(layout));
    gate.Activate();

    RealityAssignment a;
    a.Join(100);  // Past + Echo  (memory)
    a.Join(200);  // Present + Future (physical)
    SimPlayer memory{100, PhaseState(a.Find(100)->home, PhaseConfig{})};
    SimPlayer physical{200, PhaseState(a.Find(200)->home, PhaseConfig{})};

    // Memory player must visit Echo to learn the socket mapping...
    CHECK_EQ(memory.phase.RequestPhase(Reality::Echo, a.Find(100)->covered), PhaseResult::Ok);
    // ...and cannot help operate the panel (Present belongs to the other player).
    CHECK_EQ(memory.phase.RequestPhase(Reality::Present, a.Find(100)->covered),
             PhaseResult::OnCooldown);
    memory.phase.Tick(3.0f);
    CHECK_EQ(memory.phase.RequestPhase(Reality::Present, a.Find(100)->covered),
             PhaseResult::NotPermitted);

    for (int glyph : layout.glyphSequence) {
        CHECK_EQ(Press(gate, physical, kMeridianSocketInput, layout.socketForGlyph[glyph]),
                 SubmitResult::Accepted);
    }
    // Physical player phases to Future to watch the surge, then must return
    // to Present: the Engage lever is not operable from Future.
    CHECK_EQ(physical.phase.RequestPhase(Reality::Future, a.Find(200)->covered), PhaseResult::Ok);
    RunSurgeUntilCalm(gate);
    CHECK_EQ(Press(gate, physical, kMeridianEngageInput, 1), SubmitResult::WrongReality);
    physical.phase.Tick(3.0f);
    CHECK_EQ(physical.phase.RequestPhase(Reality::Present, a.Find(200)->covered), PhaseResult::Ok);
    CHECK_EQ(Press(gate, physical, kMeridianEngageInput, 1), SubmitResult::Completed);
}

TEST(Sim_DisconnectMidPuzzleKeepsProgressAndCoverage) {
    const MeridianGateLayout layout = GenerateMeridianGateLayout(7);
    PuzzleInstance gate(BuildMeridianGatePuzzle(layout));
    gate.Activate();
    RealityAssignment a;
    for (PlayerId p = 1; p <= 4; ++p) a.Join(p);
    SimPlayer present{2, PhaseState(Reality::Present, PhaseConfig{})};
    Press(gate, present, kMeridianSocketInput, layout.socketForGlyph[layout.glyphSequence[0]]);
    Press(gate, present, kMeridianSocketInput, layout.socketForGlyph[layout.glyphSequence[1]]);

    // Present player drops; puzzle state lives on the server and is untouched.
    a.Leave(2);
    CHECK_EQ(gate.Progress(), 2);
    CHECK_EQ(a.CoveringPlayer(Reality::Present), 3u);  // Future player inherits Present

    // Future player phases to Present and finishes the sequence.
    SimPlayer future{3, PhaseState(a.Find(3)->home, PhaseConfig{})};
    CHECK_EQ(future.phase.RequestPhase(Reality::Present, a.Find(3)->covered), PhaseResult::Ok);
    Press(gate, future, kMeridianSocketInput, layout.socketForGlyph[layout.glyphSequence[2]]);
    Press(gate, future, kMeridianSocketInput, layout.socketForGlyph[layout.glyphSequence[3]]);
    RunSurgeUntilCalm(gate);
    CHECK_EQ(Press(gate, future, kMeridianEngageInput, 1), SubmitResult::Completed);

    // Original player reconnects afterwards and gets their home back.
    CHECK_EQ(a.Rejoin(2, Reality::Present), JoinResult::Ok);
    CHECK_EQ(a.Find(2)->home, Reality::Present);
}
