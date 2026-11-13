# SPLIT REALITY — The Silence Between Seconds

*Four Players. Four Realities. One World.*

A cinematic co-op sci-fi mystery for Unreal Engine 5. Four players stand in the
same frozen city, Asterion at 11:47 PM in 2479, and each one sees a different
reality: **Past, Present, Future or Echo**. No one can finish alone.

## Repository layout
```
Core/SRCore/      Engine-independent gameplay rules (C++17) + tests   ✅ builds, 32 tests pass
Documentation/    Vision, bibles, design, architecture, plans, status
Game/             (to come) Unreal project — needs a UE5 workstation
```

## Build and test the core (any Linux/macOS/Windows machine with CMake)
```bash
cmake -S Core/SRCore -B build/srcore -DCMAKE_BUILD_TYPE=Debug
```
```bash
cmake --build build/srcore
```
```bash
./build/srcore/srcore_tests
```

## Status
Phase 0 (audit) and Phase 1 (foundation) are done. Phase 2 (first playable
environment) is **blocked**: it needs a Windows machine with Unreal Engine 5
and a GPU. See `Documentation/DEVELOPMENT_STATUS.md` and `Documentation/DECISIONS.md`.

## Start here
1. `Documentation/GAME_VISION.md`
2. `Documentation/GAMEPLAY_DESIGN.md`
3. `Documentation/TECHNICAL_ARCHITECTURE.md`
4. `Documentation/ROADMAP.md`

## Next steps
1. **Owner:** provide a UE5 workstation and decide D-05 to D-08 (`Documentation/DECISIONS.md`).
2. **On the workstation:** create the C++ Third Person project in
   `Game/SplitReality`, move SRCore in as a module (TECHNICAL_ARCHITECTURE.md §1),
   and confirm the build and tests pass there.
3. Build `ASRPlayerState` reality replication and `URealityComponent` in a graybox map (Phases 2–3).
