# Gameplay Design

Rules marked **[SRCore]** are implemented and unit-tested in
`Core/SRCore` (see TEST_PLAN.md). Everything else is design that still has to be built.

## 1. Core loop
Explore your reality → find a fragment of a problem → **describe it** →
combine descriptions → act (often one player acting on others' information)
→ the world changes in every reality → a new question.

## 2. Reality anchoring and coverage [SRCore]
- Each player has a **home reality**, the one they live in.
- Each reality is **covered** by exactly one player at all times.
- With 4 players: one reality each. Joining order assigns Past, Present, Future, then Echo.
- **Fallback pairing:** memory realities (Past + Echo) pair together, and
  physical realities (Present + Future) pair together.
  - 2 players: one covers Past + Echo, the other covers Present + Future.
    The clue holder is never the machine operator, so talking stays mandatory.
  - 3 players: the Past player also covers Echo.
  - 1 player (development/solo test only): covers everything.
- When someone leaves, the others **keep their homes**. The orphaned reality goes
  to the partner's holder, otherwise to the least-loaded player.
- A reconnecting player gets their previous home back if it is still free.

*Why pairs instead of free switching?* If anyone could phase anywhere, the
information asymmetry would collapse into one player looking at everything.

## 3. Phasing (reality switching) [SRCore]
- A player can **phase** into any reality they cover, or one granted temporarily by the mission.
- Values (tunable in data): 100 energy, 15 to activate, drain 8/s while
  away, regen 12/s at home, 20 minimum to leave home, 3 s cooldown.
- When energy runs out the player **snaps home** (forced return, with a
  strong audio/visual cue). Going home is always free.
- With 4 players, phasing is mostly unused. Missions can grant temporary
  access at designed **Convergence Points** for set-piece moments.
- The game layer must also reject a phase that would put the player inside
  geometry that exists only in the target reality ("Obstructed"). That check
  needs the engine's collision queries, so it lives in the UE layer.

## 4. Perception rule [SRCore: `IsPerceivable`]
An object belongs to a set of realities. A player perceives it only while
perceiving one of those realities. **One predicate drives three things**: rendering
visibility, collision and network relevancy. They can never disagree, and a
modified client cannot receive data for objects in realities it does not perceive.

## 5. Cooperative puzzle framework [SRCore]
Data-driven `PuzzleDefinition`:
- **Inputs**, each operable from exactly one reality.
- **Solution**, an ordered list of steps (input + value + optional condition).
- **Conditions**, named flags raised by world systems (for example, "SurgeCalm").
- **Mistakes** reset progress. N mistakes cause a timed **lockout**
  (narratively, MORROW intervening).
- Submissions from the wrong reality are rejected but do **not** count as
  mistakes, because the UI should never have offered them.
- **Replay protection:** request ids increase per player, so a retransmitted
  network call can never press a button twice.
- Simultaneous presses are resolved in server arrival order (deterministic).

Puzzle design checklist (each one must pass all of these):
1. Needs information from at least 3 realities with 4 players.
2. Remains solvable with 2 players using the pairing and phasing rules.
3. No single reality contains both the question and the answer.
4. Has a session seed so solutions cannot be copied from a video.
5. Every failure state has clear audiovisual feedback in every reality.

## 6. Chapter 1 puzzle: The Meridian Gate [SRCore: generator + rules]
| Reality | What they see | What they contribute |
|---|---|---|
| Past | The founders' frieze: 4 of 6 glyphs, in order | The **sequence** |
| Echo | Filaments linking each glyph to one of six sockets | The **mapping** glyph → socket |
| Present | Six burned, unlabelled sockets and the Engage lever | The **hands** |
| Future | The gate's surge arcs; a calm window every 9–14 s, lasting 2.5–4 s | The **timing**: "NOW" |

Solution: press four sockets in glyph order, then pull Engage during a calm
window. Three mistakes cause a 20 s lockout (MORROW's first direct
intervention). The sequence, mapping and surge timing are generated from the
session seed.

**Two-player version:** the memory player phases between Past and Echo to
build the socket order and must manage energy. The physical player phases
into Future to read the surge, then returns to Present, because Engage can
only be operated from Present. Covered by the test `Sim_TwoPlayerFallbackSolvesWithPhasing`.

## 7. Reality-specific interactions in the slice (one minimum each)
- **Past:** read the dedication lectern (an audio recording of Elara's founding-day
  speech; it reveals that the glyphs are "the order the founders lit the harbour lamps").
- **Present:** carry a power cell from the flooded lobby to the gate panel to
  restore power. Physical object, carried and replicated.
- **Future:** foresee the causeway collapse. Unstable slabs glow for the
  Future player, who guides the others along the safe route. Stepping on a
  marked slab in any reality triggers a fall and a respawn at a checkpoint.
- **Echo:** follow a filament from a frozen citizen to a hidden switch that
  unseals the Hall of Founding in the Present.

## 8. Communication support
- Voice chat (platform/EOS) is expected but **not required**.
- **Contextual ping wheel:** "Here", "Wait", "Now!", "Danger", "Go", plus 6
  glyph pings. A ping marks a world position visible in every reality, even
  though the object being pinged is not.
- Pings make the game playable without voice and accessible to deaf and
  hard-of-hearing players.

## 9. Mission system (design)
Objectives are data assets containing an ordered list of objective states with
cooperative requirements (for example, "all present players at the gate").
Checkpoints save mission and puzzle state on the server. Failure states (falls,
lockouts) are recoverable, with no game over in Chapter 1. Replay: the seed
changes every run.
