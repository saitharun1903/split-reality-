#include "TestHarness.h"
#include "srcore/Reality.h"
#include "srcore/RealityAssignment.h"

using namespace sr;

TEST(Mask_BasicSetOperations) {
    RealityMask m = RealityMask::Of(Reality::Past).With(Reality::Echo);
    CHECK(m.Contains(Reality::Past));
    CHECK(m.Contains(Reality::Echo));
    CHECK(!m.Contains(Reality::Present));
    CHECK_EQ(m.Count(), 2);
    CHECK_EQ(m.Without(Reality::Past), RealityMask::Of(Reality::Echo));
    CHECK_EQ(RealityMask::All().Count(), 4);
    CHECK(RealityMask::None().IsEmpty());
    CHECK_EQ(RealityMask(0xFF), RealityMask::All());  // stray bits are masked off
}

TEST(Partner_IsSymmetric) {
    for (Reality r : kAllRealities) CHECK_EQ(Partner(Partner(r)), r);
    CHECK_EQ(Partner(Reality::Past), Reality::Echo);
    CHECK_EQ(Partner(Reality::Present), Reality::Future);
}

TEST(Perceivable_OnlyInOwnRealities) {
    const RealityMask pastOnly = RealityMask::Of(Reality::Past);
    CHECK(IsPerceivable(pastOnly, Reality::Past));
    CHECK(!IsPerceivable(pastOnly, Reality::Present));
    CHECK(IsPerceivable(RealityMask::All(), Reality::Echo));
}

static void CheckEveryRealityCoveredOnce(const RealityAssignment& a) {
    for (Reality r : kAllRealities) {
        int owners = 0;
        for (const PlayerSlot& s : a.Slots()) owners += s.covered.Contains(r) ? 1 : 0;
        CHECK_EQ(owners, a.PlayerCount() == 0 ? 0 : 1);
    }
}

TEST(Assignment_FourPlayersOneEach) {
    RealityAssignment a;
    for (PlayerId p = 1; p <= 4; ++p) CHECK_EQ(a.Join(p), JoinResult::Ok);
    CHECK_EQ(a.Find(1)->home, Reality::Past);
    CHECK_EQ(a.Find(2)->home, Reality::Present);
    CHECK_EQ(a.Find(3)->home, Reality::Future);
    CHECK_EQ(a.Find(4)->home, Reality::Echo);
    for (const PlayerSlot& s : a.Slots()) CHECK_EQ(s.covered.Count(), 1);
    CheckEveryRealityCoveredOnce(a);
    CHECK_EQ(a.Join(5), JoinResult::SessionFull);
    CHECK_EQ(a.Join(2), JoinResult::AlreadyJoined);
    CHECK_EQ(a.Join(kInvalidPlayer), JoinResult::InvalidPlayer);
}

TEST(Assignment_TwoPlayerFallbackSplitsMemoryAndPhysical) {
    RealityAssignment a;
    a.Join(10);
    a.Join(20);
    CHECK_EQ(a.Find(10)->covered, RealityMask::Of(Reality::Past).With(Reality::Echo));
    CHECK_EQ(a.Find(20)->covered, RealityMask::Of(Reality::Present).With(Reality::Future));
    CheckEveryRealityCoveredOnce(a);
}

TEST(Assignment_ThreePlayersEchoGoesToPast) {
    RealityAssignment a;
    a.Join(1);
    a.Join(2);
    a.Join(3);
    CHECK_EQ(a.CoveringPlayer(Reality::Echo), 1u);
    CheckEveryRealityCoveredOnce(a);
}

TEST(Assignment_SoloCoversEverything) {
    RealityAssignment a;
    a.Join(7);
    CHECK_EQ(a.Find(7)->covered, RealityMask::All());
}

TEST(Assignment_LeaveKeepsHomesAndRedistributes) {
    RealityAssignment a;
    for (PlayerId p = 1; p <= 4; ++p) a.Join(p);
    CHECK(a.Leave(2));  // Present holder disconnects
    CHECK_EQ(a.Find(1)->home, Reality::Past);
    CHECK_EQ(a.Find(3)->home, Reality::Future);
    CHECK_EQ(a.Find(4)->home, Reality::Echo);
    CHECK_EQ(a.CoveringPlayer(Reality::Present), 3u);  // partner (Future) takes it
    CheckEveryRealityCoveredOnce(a);
    CHECK(!a.Leave(2));
}

TEST(Assignment_RejoinRestoresPreviousHome) {
    RealityAssignment a;
    for (PlayerId p = 1; p <= 4; ++p) a.Join(p);
    a.Leave(3);
    CHECK_EQ(a.Rejoin(3, Reality::Future), JoinResult::Ok);
    CHECK_EQ(a.Find(3)->home, Reality::Future);
    for (const PlayerSlot& s : a.Slots()) CHECK_EQ(s.covered.Count(), 1);
}

TEST(Assignment_RejoinWhenHomeTakenFallsBackToFree) {
    RealityAssignment a;
    a.Join(1);
    a.Join(2);
    a.Leave(1);    // Past is free
    a.Join(9);     // newcomer takes Past
    CHECK_EQ(a.Rejoin(1, Reality::Past), JoinResult::Ok);
    CHECK_EQ(a.Find(1)->home, Reality::Future);
    CheckEveryRealityCoveredOnce(a);
}

TEST(Assignment_PartnersOnlyStillCoverAll) {
    // Homes Past + Echo (partners): Present and Future have no partner holder
    // and must be spread by load, not dumped on one player.
    RealityAssignment a;
    for (PlayerId p = 1; p <= 4; ++p) a.Join(p);
    a.Leave(2);
    a.Leave(3);
    CheckEveryRealityCoveredOnce(a);
    CHECK_EQ(a.Find(1)->covered.Count(), 2);
    CHECK_EQ(a.Find(4)->covered.Count(), 2);
}
