# Audio & UX Design

## Audio direction
Audio is information, not decoration.

| Reality | Bed | Gameplay signal |
|---|---|---|
| Past | Distant crowds, gulls, clockwork, bells | Bells chime the glyph order when you stand at the frieze (an audio duplicate of the clue) |
| Present | Rain, electrical hum, drips, failing alarms | The panel hum's pitch rises as the power cell nears the gate |
| Future | Wind, groaning steel, far detonations | Surge arcs build with a rising tone, so calm windows can be *heard* as well as seen |
| Echo | Resonant drones, reversed whispers | Filaments "sing" in a pitch unique to each glyph |

- **Implementation:** MetaSounds per reality bed; a per-client "reality mix"
  submix crossfades over 0.6 s on phase. Spatialised one-shots for interactions.
- **Footsteps:** surface × reality matrix (stone/brass/glass/rubble/void).
- **Mix priority:** dialogue > critical gameplay cues > puzzle feedback > ambience > music.
- **Music:** sparse. A solo-instrument motif per reality that combines into a
  single four-part theme only when the gate opens.
- **Voice:** subtitles on by default; speaker names coloured with a shape icon; size adjustable.

## UX principles
Cinematic, minimal, readable. Thin typography, generous negative space, a
reality-tinted accent on every screen. No default Unreal widgets in shipped
UI. Every screen is navigable by mouse, controller and touch.

## Screens (slice scope marked ★)
★ Startup/legal · ★ Main menu · ★ Settings (graphics, audio, controls,
accessibility) · ★ Host/Join lobby (LAN/direct for the slice) · ★ In-game HUD
· ★ Pause · ★ Chapter complete/results · Chapter select · Matchmaking (EOS, after slice)

## In-game HUD (★)
- Top-left: reality badge (name + shape icon + colour); phase energy arc when relevant.
- Top-centre: current objective (one line).
- Bottom-centre: contextual interaction prompt (input-device aware).
- Edge indicators for teammates, showing each one's home-reality shape.
- Ping wheel (hold) with the glyph sub-menu.

## Accessibility (slice minimum)
Subtitles with size/background options; colour-blind-safe shape coding;
camera/look sensitivity and invert; hold/toggle options for sprint and ping;
remappable keys; reduced camera shake and flash; text-to-speech hook for pings (post-slice).

## Mobile (Android) UX
Virtual joystick (left), camera drag (right), context button that swaps icon,
quick-ping radial; HUD scales to the safe area; minimum 48 dp touch targets.
