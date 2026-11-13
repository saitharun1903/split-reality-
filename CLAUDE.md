# CLAUDE.md — working rules for SPLIT REALITY

## Read first
`Documentation/DEVELOPMENT_STATUS.md` (what is done, incomplete or blocked),
`Documentation/DECISIONS.md` (do not re-open accepted decisions without cause),
then the design doc for the area you are touching.

## Ground rules
- Never claim a build, test, benchmark, screenshot or asset exists unless it
  was produced and verified in this session or recorded with evidence in DEVELOPMENT_STATUS.md.
- Check for Unreal Engine before any engine work (`find / -iname "UnrealEditor*"`).
  The cloud container has **no UE, no GPU, no .NET and no Android SDK**. Engine work happens on the owner's workstation.
- Do not build later chapters before the Chapter 1 vertical slice is accepted.
- Canon is in STORY_BIBLE.md. Material marked PROPOSED needs owner approval before it ships.
- Record every third-party asset in ASSET_REGISTER.md before committing it.
- Keep documentation in sync with code in the same commit.

## SRCore (Core/SRCore)
- Pure C++17. **No Unreal headers, no exceptions, no RTTI.** Warnings are errors.
- All gameplay rules that decide visibility, permission or puzzle outcome belong here, with tests.
- Determinism: use `sr::SplitMix64`, never `std::` random distributions.
- Build and test:
  ```bash
  cmake -S Core/SRCore -B build/srcore -G Ninja && cmake --build build/srcore && ./build/srcore/srcore_tests
  ```
- Test with both GCC and Clang when changing it. For sanitizers use GCC (Clang's ASan runtime is not installed in the container).

## Unreal layer conventions (when created)
- Server authority: the client sends intent only. Build `sr::PuzzleActor` from server-side PlayerState, never from RPC parameters.
- One perception predicate (`sr::IsPerceivable`) for visibility, collision and relevancy.
- C++ for systems; Blueprints for content wiring; Data Assets for puzzles, dialogue and missions.
- Classes are prefixed `SR` (`ASRCharacter`, `URealityComponent` is the exception, kept for readability).
