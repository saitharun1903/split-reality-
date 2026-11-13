# Test Plan

## Level 1 — SRCore unit and rule-simulation tests ✅ running
Location: `Core/SRCore/tests`. No Unreal required.

```bash
cmake -S Core/SRCore -B build/srcore -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/srcore && ./build/srcore/srcore_tests
```

**Last verified run (2026-11-13, this container):** 32 tests, 0 failures,
on GCC 13.3 and Clang 18.1.3 in Debug and Release with `-Werror
-fno-exceptions -fno-rtti`, plus GCC AddressSanitizer + UBSan.

Coverage: reality mask/partner/perception; assignment for 1–4 players,
leave, rejoin and partner fallback; phase energy, drain, forced return,
regen, threshold and cooldown; puzzle validation, cross-reality solve,
wrong-reality rejection, condition mistakes, lockout and recovery, replay
protection, simultaneous presses, reset; Meridian Gate determinism,
well-formedness over 500 seeds, variety; end-to-end rule simulations for 4
players, a surge mistake, a 2-player fallback with phasing, and a disconnect
mid-puzzle with rejoin.

## Level 2 — Unreal automation tests (planned)
- Mirror SRCore tests as UE Automation Specs once SRCore is a UE module, so they also run inside UBT builds.
- Functional tests in a test map: phasing toggles visibility and collision; interaction
  rejected from the wrong reality; replicated actor not relevant to the wrong reality.

## Level 3 — Multiplayer (Phase 4, PIE + packaged)
PIE with 4 clients ("Play As Listen Server"), then packaged builds on separate machines.
Scenarios: full 4-player solve · 2-player solve · 3-player · drop mid-puzzle +
rejoin · host drop + re-host from checkpoint · simultaneous presses ·
wrong-reality press via console cheat (must be rejected server-side) · network
emulation 150 ms / 2 % loss.

## Level 4 — Vertical-slice playtest procedure (repeatable)
1. Fresh packaged build, settings reset, 4 testers (then 2), voice allowed.
2. Observe without helping; record time per beat, mistakes, lockouts, and moments of confusion.
3. Post-session survey: "Which reality were you in? What did you see that others didn't?"
4. Capture an Unreal Insights trace on the host for one full session → PERFORMANCE_BUDGET.md.

## Level 5 — Later
Memory soak, loading, accessibility audit, security review of RPC surface,
Android device matrix, Pixel Streaming latency tests.
