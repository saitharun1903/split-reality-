# Decision Log

Status: **Accepted** (in effect) · **Recommended** (studio proposal; proceed
unless the owner objects) · **Owner decision** (blocked on approval).

| ID | Decision | Status | Rationale / alternatives |
|---|---|---|---|
| D-01 | Gameplay rules live in engine-independent C++ (`SRCore`), wrapped by a thin UE layer | Accepted | Testable without UE, deterministic, reusable for a WASM/web client. Alternative (all rules in UObjects) is only testable inside the editor |
| D-02 | Slice uses **listen servers**; dedicated servers deferred | Recommended | Works with the Launcher engine; no hosting. Cost: a host drop ends the session (mitigated by client-side checkpoints) |
| D-03 | Reality coverage with Past↔Echo / Present↔Future pairing; phasing limited to covered + granted realities | Accepted (design) | Keeps information asymmetry with any player count; enables the 2-player fallback |
| D-04 | Online: `OnlineSubsystemNull` for the slice, **EOS** afterwards | Recommended | EOS is free and covers PC and Android; Steam can layer on later for distribution |
| D-05 | Web = **Pixel Streaming for limited access**; no browser-native client | Owner decision | See PLATFORM_STRATEGY.md comparison. Per-player GPU cost makes open free web play expensive |
| D-06 | Android starts after PC slice acceptance with a 2-week rendering spike | Owner decision | Four dynamic lighting rigs are the hardest part to bring to mobile |
| D-07 | Identity of the figure at the end of Chapter 1 (Elara vs Nyra) | Owner decision | Elara pays off "her true fate is a mystery"; Nyra sets up Ch3 |
| D-08 | Proposed canon additions (anchor crew, MORROW's origin, backward warning, Kael's early start, frozen citizens) | Owner decision | STORY_BIBLE.md "Proposed" section |
| D-09 | **Provide a UE5 workstation** (Windows, DX12 GPU ≥ 8 GB VRAM, 32–64 GB RAM) and pin the engine version | Owner action: in progress (2026-11-13: owner is acquiring a machine) | Nothing visual or engine-side can be built or verified without it |
| D-10 | PC rendering: Lumen + Nanite + VSM + TSR on High/Epic, Lumen-off Low tier | Recommended | Required for runtime-switchable lighting per reality |
| D-11 | Replication: legacy replication + `IsNetRelevantFor` for the slice; Iris later if needed | Recommended | Simpler and well documented at 4 players |
| D-12 | No Gameplay Ability System, no World Partition in the slice | Recommended | Complexity without benefit at slice scope; revisit in Phase 6 |
| D-13 | Player characters ("the Witnesses") are cosmetic; reality is assigned by join order, not character | Recommended | Players never have to fight over a character to get a role |
