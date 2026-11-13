# Cinematic Design

Tools: Sequencer, Cine Camera Actors, Movie Render Queue (trailers only),
Control Rig for additive performance, MetaSounds for scored stingers.
All runtime cinematics are **real-time in-engine**. Pre-rendered video is used
only for trailers.

## Rules
- Each cinematic has a gameplay reason (orientation, motivation, reward).
- The first view is unskippable only for the first 10 s, then skippable. Repeat views are always skippable.
- All dialogue is driven by a localisation-ready **Dialogue Data Table**
  (line id, speaker, text key, audio asset, subtitle timing).
- Multiplayer: cinematics play locally on every client, triggered by a
  replicated server event with a start timestamp. Late joiners skip to the current state.
- Per-reality cinematics: in shared moments, each client renders the same
  camera move in its own reality. This is our signature shot language.

## The Last Signal (opening) — shot list v1
| # | Dur | Picture | Sound |
|---|---|---|---|
| 1 | 4 s | Black | Distant electrical crackle, slowly rising |
| 2 | 5 s | Black. Subtitle appears with the line | Elara (filtered radio): *"If anyone can hear this... do not activate the machine."* |
| 3 | 3 s | Clock digits fade in: **11:46 PM** | A single low piano note; the hum continues |
| 4 | 20 s | Aerial push over the sea toward Asterion at night; the Spire on the horizon | Sea wind, then the city murmur builds |
| 5 | 25 s | Low drone glide through the Concourse: people walking, trams, warm windows; shallow depth of field on a child pointing at the Spire | Crowd walla, tram bells, music swells |
| 6 | 10 s | The clock tower; second hand sweeping; cut to Spire core lighting | Ticking slowly comes forward in the mix |
| 7 | 3 s | **11:47 PM** | Ticking stops |
| 8 | 4 s | The Engine fires: a vertical beam, then *everything freezes*, including rain droplets and a bird mid-flight | **Total silence** (the "silence between seconds") |
| 9 | 6 s | An energy wave rolls across the frozen city; the camera holds still | Sub-bass swell, reversed crowd sound |
| 10 | 8 s | Screen splits into four quadrants, each the same street in a different reality, then they slide and overlap, distorting | Four ambiences layered, phasing against each other |
| 11 | 6 s | Hard cut to black → **SPLIT REALITY** title, then the subtitle *The Silence Between Seconds* | One resonant chord |

Total ≈ 1:34 (plus title). Production cost driver: crowds in shot 5. Plan:
Mass Crowd or animated instanced crowds at mid-distance, with 6–10 hero
characters in the foreground. Budget this as the most expensive shot in the slice.

## Other slice cinematics
- **Reality-transition sting** (gameplay, < 1 s): a radial "tearing" distortion,
  the outgoing reality's sound sucked away, the incoming one blooming in.
  Used for phasing and forced returns.
- **MORROW introduction** (in-engine, interactive): the gate PA crackles, lights
  dim in a 4-beat pattern, MORROW's ring motif appears on every screen in
  every reality. Players keep control.
- **Chapter end** (beat 7): see CAMPAIGN_DESIGN.md.
