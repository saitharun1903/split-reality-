# Technical Architecture

## 1. Layering
```
┌──────────────────────────────────────────────────────────────┐
│ Content (Blueprints, Data Assets, levels, Sequencer, audio)  │  designers
├──────────────────────────────────────────────────────────────┤
│ SplitReality game module (UE C++)                            │  networking,
│   GameMode/GameState/PlayerState, Character, components,     │  presentation,
│   replication, RPCs, input, UI, subsystems                   │  engine glue
├──────────────────────────────────────────────────────────────┤
│ SRCore (pure C++17, no engine headers)          ✅ built     │  rules: assignment,
│   Reality, RealityAssignment, PhaseState,       ✅ tested    │  phasing, puzzles,
│   CoopPuzzle, MeridianGate                                   │  seeded content
└──────────────────────────────────────────────────────────────┘
```
**Why a pure C++ core?** The rules that decide who can see, do and solve what
are the most important code in the game and the easiest to get subtly wrong
in multiplayer. Keeping them engine-free means they are:
- unit-tested in milliseconds on any machine, including CI and this container;
- deterministic (custom PRNG, no platform-dependent `std::` distributions);
- written to UE constraints (no exceptions, no RTTI, warnings as errors).

The UE layer only translates between engine types and SRCore types.

### Integrating SRCore into Unreal (first task on the UE workstation)
Move `Core/SRCore` to `Game/SplitReality/Source/SRCore/` (`include/` → `Public/`,
`src/` → `Private/`) and add `SRCore.Build.cs` (Type: Runtime, no engine
dependencies beyond `Core`) plus a one-line module file
(`IMPLEMENT_MODULE(FDefaultModuleImpl, SRCore)`). Point CMake at the new
location so the fast tests keep working. *Not done yet, because it can only be
verified with UnrealBuildTool.*

## 2. UE game module (planned classes)
| Class | Responsibility | Authority |
|---|---|---|
| `ASRGameMode` | Owns `sr::RealityAssignment`, session seed, mission flow; handles join/leave/reconnect | Server only |
| `ASRGameState` | Replicates mission state, objective, puzzle progress summary | Server → all |
| `ASRPlayerState` | Replicates `HomeReality`, `CurrentReality`, `CoveredMask`, energy; holds the server's `sr::PhaseState` | Server → all |
| `ASRCharacter` | Third-person movement (CharacterMovement), interaction trace, phase input → `ServerRequestPhase` | Client predicts, server decides |
| `URealityComponent` | Gives any actor a reality mask; applies local visibility and collision; overrides relevancy | Both |
| `USRRealityViewSubsystem` (LocalPlayer subsystem) | Applies the local reality: lighting rig, `MPC_Reality`, post, audio mix, transition FX | Client only |
| `UInteractionComponent` + `ISRInteractable` | Focus/prompt on the client; `ServerInteract(Target, InputId, Value, RequestId)` | Server validates |
| `ASRPuzzleActor` | Wraps `sr::PuzzleInstance`; raises conditions from world systems; broadcasts results | Server only |
| `USRPuzzleDefinitionAsset` | Data asset → `sr::PuzzleDefinition` (validated with `sr::Validate` on save) | Editor/data |
| `ASRMeridianClueActor` (×4) | Reality-specific actors that receive *only their part* of the seeded layout | Server → relevant clients only |

## 3. The perception pipeline
One predicate (`sr::IsPerceivable(objectMask, viewerReality)`) drives:
1. **Rendering:** `URealityComponent` sets primitive visibility on the local client
   whenever the local player's `CurrentReality` changes (OnRep).
2. **Collision:** four custom object channels `Reality_Past/Present/Future/Echo`.
   Reality-only blockers use their channel. The pawn capsule blocks only the
   channel of its current reality. Base geometry is WorldStatic and blocks
   everyone. The server simulates movement with the same channels, so
   collision is server-authoritative.
3. **Replication relevancy:** reality-specific replicated actors override
   `AActor::IsNetRelevantFor` and use the viewer's `CurrentReality`. **Seeded
   solution data is held only in reality-specific actors**, so a modified
   client never receives, for example, the Past glyph sequence unless it
   perceives the Past.
   *(If Iris replication is enabled later, port this to an Iris filter.
   Decision D-11.)*

Static art that exists in only one reality ships in the client's package
(it can be extracted by data-mining). That is acceptable: the *solution* is seeded at runtime.

### Listen-server caveat
The host machine has all realities loaded for simulation. The host's local
view hides other realities through the same `URealityComponent` path. The
host could cheat; for a co-op game that is acceptable and documented.

## 4. Network flows
- **Phase:** input → `ServerRequestPhase(Target)` → `PhaseState::RequestPhase` +
  obstruction check (overlap test against the target reality's channel) →
  replicated `CurrentReality` → OnRep on every client (visibility, collision,
  audio, FX). The owning client plays the transition FX immediately and
  reconciles if the server rejects.
- **Interaction/puzzle:** the client sends only *intent* (target, input, value,
  request id). The server builds `PuzzleActor{player, CurrentReality}` from
  **its own** PlayerState and never from client claims, then calls `Submit`.
  The result is replicated to all relevant clients for feedback.
- **Ticks:** `PhaseState::Tick` and `PuzzleInstance::Tick` run on the server
  (energy is replicated at a low frequency; the client interpolates for UI).

## 5. Engine feature choices (PC "High" tier; Android in PLATFORM_STRATEGY.md)
| Feature | Use? | Reason |
|---|---|---|
| Lumen GI/reflections | Yes (High/Epic), off on Low | Four lighting rigs that change at runtime cannot be pre-baked on PC; Lumen makes per-reality lighting viable |
| Nanite | Yes for static environment meshes | Dense architecture and kitbash; reduces LOD authoring |
| Virtual Shadow Maps | Yes with Nanite; regular shadow maps on Low | Pairs with Nanite |
| TSR | Yes | Upscaling to hit frame budget |
| World Partition | **No for the slice** (one hand-built district); revisit at Phase 6 | Overhead without benefit at this size |
| Niagara | Yes | All VFX |
| MetaSounds | Yes | Per-reality beds, procedural surge tones |
| Sequencer | Yes | All cinematics |
| Control Rig | Limited (cinematic polish) | Avoid a rigging pipeline before it is needed |
| Gameplay Ability System | **No** | No combat or ability trees; phasing and interaction are simpler as plain components. Revisit if combat arrives |
| Gameplay Tags | Yes | Interaction types, objective ids, reality tags in data |
| Enhanced Input | Yes | Keyboard/mouse, controller and touch mapping contexts |
| Iris replication | Not for the slice | Legacy replication + `IsNetRelevantFor` is sufficient for 4 players and better documented |

## 6. Backend
**None for the vertical slice.** LAN/direct-IP listen servers through
`OnlineSubsystemNull`. After the slice: **EOS** (lobbies, P2P relay for NAT,
identity on PC/Android, achievements). A custom backend (TypeScript/Node +
PostgreSQL) is introduced only for things EOS does not cover, such as
cross-session progression or a web client protocol. DATABASE_DESIGN.md will
be written when that is decided. **Never trust client-reported rewards,
results or purchases.**
