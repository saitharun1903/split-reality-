# Development Status

_Last updated: 2026-11-13_

## Completed (verified)
| Item | Evidence |
|---|---|
| Phase 0 audit | ENVIRONMENT_AUDIT.md (commands run in the container) |
| Repository, `.gitignore`, Git LFS rules | Git history |
| SRCore: realities, assignment, phasing, co-op puzzle framework, Meridian Gate generator | `Core/SRCore`; 32 tests pass (GCC 13 & Clang 18, Debug/Release, `-Werror`, no exceptions/RTTI; GCC ASan+UBSan clean) |
| Foundation documents | `Documentation/` |

## Incomplete
| Item | Notes |
|---|---|
| Unreal project (`Game/SplitReality`) | Not created: no engine here. Creating an unverified `.uproject` and C++ module would break the "never claim a build" rule |
| UE classes in TECHNICAL_ARCHITECTURE.md §2 | Designed, not written |
| DATABASE_DESIGN.md, Backend/, WebClient/ | Intentionally deferred (no validated need yet) |
| Art, audio, cinematics, UI | No assets exist |

## Blocked
| Item | Blocked by |
|---|---|
| Phases 2–9 | **D-09: no Unreal-capable machine** (no UE, GPU, Windows/.NET toolchain or Android SDK in the cloud container) |
| Narrative lock for Chapter 1 ending | D-07, D-08 owner decisions |
| Web and Android plans | D-05, D-06 owner decisions |

## Risk register
| # | Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|---|
| R1 | No UE workstation available | Certain today | Blocks all engine work | Owner provides a machine (D-09); meanwhile build engine-free core and docs |
| R2 | AAA visual target vs team size and budget | High | High | Graybox-first; licensed kits; one district only; hire/commission environment and character art for the slice |
| R3 | Puzzle unclear without voice | Medium | High | Ping wheel with glyph pings; early 2-player playtests |
| R4 | Per-reality lighting too costly (PC Low, Android) | Medium | High | Lumen-off Low tier; Android spike on Lighting Scenarios |
| R5 | Collision changes on phase cause movement corrections | Medium | Medium | Server obstruction check before phasing; test under latency |
| R6 | Host drop ends the session | High (listen server) | Medium | Client-side checkpoints; dedicated servers later if needed |
| R7 | Pixel Streaming cost | High if web is open | High | Limited-access web only (D-05) |
| R8 | Asset licensing errors | Medium | High | ASSET_REGISTER.md before commit |
| R9 | Scope creep into later chapters | Medium | High | Roadmap gates; Chapter 2 work forbidden until slice acceptance |

## Next action
See README.md "Next steps".
