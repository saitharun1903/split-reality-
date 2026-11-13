#include "TestHarness.h"
#include "srcore/PhaseState.h"

using namespace sr;

static const RealityMask kPastEcho = RealityMask::Of(Reality::Past).With(Reality::Echo);

TEST(Phase_StartsHomeWithFullEnergy) {
    PhaseState s(Reality::Past, PhaseConfig{});
    CHECK_EQ(s.Current(), Reality::Past);
    CHECK(!s.IsAway());
    CHECK_NEAR(s.Energy(), 100.0f, 1e-4);
}

TEST(Phase_PermittedPhaseCostsEnergyAndStartsCooldown) {
    PhaseConfig cfg;
    PhaseState s(Reality::Past, cfg);
    CHECK_EQ(s.RequestPhase(Reality::Echo, kPastEcho), PhaseResult::Ok);
    CHECK_EQ(s.Current(), Reality::Echo);
    CHECK_NEAR(s.Energy(), cfg.maxEnergy - cfg.activationCost, 1e-4);
    CHECK_NEAR(s.CooldownRemaining(), cfg.cooldownSeconds, 1e-4);
    CHECK_EQ(s.RequestPhase(Reality::Past, kPastEcho), PhaseResult::OnCooldown);
}

TEST(Phase_RejectsUncoveredReality) {
    PhaseState s(Reality::Past, PhaseConfig{});
    CHECK_EQ(s.RequestPhase(Reality::Present, kPastEcho), PhaseResult::NotPermitted);
    CHECK_EQ(s.Current(), Reality::Past);
    CHECK_EQ(s.RequestPhase(Reality::Past, kPastEcho), PhaseResult::AlreadyThere);
}

// Phase away and tick in small steps until the player is pulled home.
// Returns elapsed seconds.
static float DrainUntilForcedHome(PhaseState& s) {
    float t = 0.0f;
    while (s.IsAway() && t < 1000.0f) {
        s.Tick(0.1f);
        t += 0.1f;
    }
    return t;
}

TEST(Phase_DrainsAwayAndForcesReturn) {
    PhaseConfig cfg;
    PhaseState s(Reality::Present, cfg);
    s.RequestPhase(Reality::Future, RealityMask::All());
    // 85 energy left at 8/s drain -> pulled home after ~10.6 s.
    for (int i = 0; i < 100; ++i) s.Tick(0.1f);
    CHECK(s.IsAway());
    const float more = DrainUntilForcedHome(s);
    CHECK(more > 0.5f && more < 0.8f);
    CHECK_EQ(s.ForcedReturnCount(), 1u);
    CHECK_NEAR(s.Energy(), 0.0f, 1e-4);
    CHECK_NEAR(s.CooldownRemaining(), cfg.cooldownSeconds, 1e-4);
}

TEST(Phase_RegeneratesAtHomeAndEnforcesThreshold) {
    PhaseConfig cfg;
    PhaseState s(Reality::Present, cfg);
    s.RequestPhase(Reality::Future, RealityMask::All());
    DrainUntilForcedHome(s);
    s.Tick(1.0f);  // 12 energy, still cooling down
    CHECK_NEAR(s.Energy(), 12.0f, 0.01);
    CHECK_EQ(s.CanPhase(Reality::Future, RealityMask::All()), PhaseResult::OnCooldown);
    s.Tick(2.0f);  // 36 energy, cooldown over, above the 20 threshold
    CHECK_EQ(s.CanPhase(Reality::Future, RealityMask::All()), PhaseResult::Ok);

    // With slow regen, the cooldown ends before the 20-energy threshold is met.
    PhaseConfig slow = cfg;
    slow.regenPerSecond = 5.0f;
    PhaseState u(Reality::Present, slow);
    u.RequestPhase(Reality::Future, RealityMask::All());
    DrainUntilForcedHome(u);
    u.Tick(3.0f);  // cooldown over, only 15 energy
    CHECK_EQ(u.CanPhase(Reality::Future, RealityMask::All()), PhaseResult::InsufficientEnergy);
    CHECK_EQ(u.CanPhase(Reality::Present, RealityMask::All()), PhaseResult::AlreadyThere);
}

TEST(Phase_EnergyNeverExceedsMax) {
    PhaseState s(Reality::Echo, PhaseConfig{});
    s.Tick(1000.0f);
    CHECK_NEAR(s.Energy(), 100.0f, 1e-4);
    s.Tick(-5.0f);  // negative delta ignored
    CHECK_NEAR(s.Energy(), 100.0f, 1e-4);
}
