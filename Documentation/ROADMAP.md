# Development Roadmap

Phases run in order. Each has **exit criteria**; a phase is complete only
when they are verified and recorded in DEVELOPMENT_STATUS.md. No calendar
dates are given until team size and hardware are known.

| Phase | Goal | Exit criteria | State |
|---|---|---|---|
| 0 | Repository & environment audit | Audit, risk report, recommended config | ✅ Done (ENVIRONMENT_AUDIT.md) |
| 1 | Game foundation | Bibles, gameplay/multiplayer/platform specs, budgets, roadmap, tested core rules | ✅ Documents and SRCore done; owner decisions pending (DECISIONS.md) |
| 2 | First playable environment | UE project builds; SRCore integrated as a module; third-person character, camera, interaction; graybox Concourse with lighting, audio and basic UI; packaged Windows build runs | ⛔ Blocked on D-09 (UE workstation) |
| 3 | Four realities | `URealityComponent` visibility, collision and relevancy; 4 lighting rigs, `MPC_Reality`, audio beds; phasing with transition FX; validated in 2+ client PIE | Not started |
| 4 | Cooperative puzzle | Meridian Gate fully playable; test matrix in TEST_PLAN.md Level 3 passes | Not started |
| 5 | Cinematic vertical slice | Opening cinematic, MORROW sequence, ending, lobby → mission → results, end to end; slice Definition of Done met | Not started |
| 6 | Chapter 1 production | Expanded spaces and puzzles after slice acceptance | Not started |
| 7 | Platform validation | PC packaged; Android spike + build; Pixel Streaming prototype | Not started |
| 8 | QA | Functional, multiplayer, performance, memory, accessibility, security | Not started |
| 9 | Release preparation | Store assets, trailer, website, privacy policy, beta plan | Not started |

## Vertical slice — Definition of Done (verbatim targets)
1 polished playable Concourse area · 1 third-person character · 4 visually
distinct realities · working phasing · ≥ 1 meaningful interaction per reality ·
the Meridian Gate co-op puzzle · 4-player sync · 2-player fallback · basic
lobby and mission entry · opening cinematic · MORROW dialogue sequence ·
mission completion sequence · working UI and audio · repeatable test
procedure · documented performance report from real profiling.
Playable start to finish. Screenshots, renders or code alone do not count.

## Slice build order inside Phases 2–5 (graybox first, art last)
1. Graybox Concourse + character + interaction (single player).
2. Multiplayer join/leave + reality assignment + replication of reality state.
3. Perception pipeline (visibility/collision/relevancy) in graybox.
4. Meridian Gate in graybox; **first 4-player and 2-player playtests**.
5. Fix the fun. Only then: art pass, lighting rigs, audio, VFX.
6. Cinematics, UI polish, performance pass, report.

## Monetisation & player trust
Nothing before the core game works. Afterwards, cosmetic only (outfits,
phase-effect variants) plus optional story expansions and premium editions.
Never sell gameplay advantages, energy, hints or anything that affects co-op balance.
