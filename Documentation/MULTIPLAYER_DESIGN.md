# Multiplayer Design

## Topology
- **Slice:** listen server (one player hosts), up to 4 players, LAN/direct IP
  (`OnlineSubsystemNull`). Works with the Launcher engine build.
- **After slice:** EOS lobbies + P2P relay for NAT traversal.
- **Dedicated servers:** only if needed (host-migration pain, public
  matchmaking). They need a source-built engine and hosting cost. Decision deferred (D-02).

## Session lifecycle
1. **Host** creates a session (chapter, checkpoint, privacy) → lobby map.
2. **Join** by invite or IP. The lobby shows players and their Witness; realities
   are assigned in join order (SRCore rules) and shown before launch.
3. **Launch** (host, ≥ 1 player; a warning below 2): server generates the session seed → travel to the mission map (seamless travel).
4. **Play:** server-authoritative state (GAMEPLAY_DESIGN.md, TECHNICAL_ARCHITECTURE.md §4).
5. **Complete:** results screen → back to lobby.

## Disconnects and reconnection
| Event | Behaviour | Status |
|---|---|---|
| Client drops | Their reality's coverage moves to the partner holder; puzzle/mission progress is untouched | Rules ✅ SRCore-tested |
| Client rejoins (same unique net id) | Gets previous home if free; inactive PlayerState restored (energy, position at last checkpoint) | Rules ✅; UE glue planned |
| Host drops | Session ends for everyone. Mitigation: every client stores the last checkpoint (seed + mission + puzzle state, a few hundred bytes), so any player can re-host from it | Planned |
| Late join mid-mission | Allowed; takes a free home reality; skips to the current state | Rules ✅ |

## Server validation checklist
- Phase requests: coverage/grant, energy, cooldown, obstruction.
- Interactions: actor's *server-side* current reality, distance and line of sight to the target, puzzle state, request-id ordering.
- Movement: standard CharacterMovement server correction.
- Never accept client-sent puzzle results, mission completion or rewards.

## Bandwidth (initial target, to be measured)
≤ 20 KB/s per client at steady state; phasing and puzzle events are rare,
reliable and small. Character movement dominates.

## Test matrix (Phase 4)
4 players · 3 players · 2 players · 1 player (dev) · drop and rejoin mid-puzzle
· host drop · wrong-reality interaction (tampered client) · simultaneous
conflicting presses · duplicate/retransmitted RPCs · 150 ms latency + 2 %
packet loss (UE network emulation). Details in TEST_PLAN.md.
