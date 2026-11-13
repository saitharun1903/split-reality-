# Art Direction

## Identity: "Four lenses on one street"
Grounded, photographic realism, where each reality is defined by
**condition, light and matter**, not by a filter. Reference language is real
architecture (Mediterranean harbour towns, Brutalist civic plazas, Japanese
transit design) and film lighting, not other games.

## How realities differ (beyond post-processing)
Each layer below is implemented in the engine so the realities stay cheap to maintain:
1. **Shared base geometry** (silhouette and landmarks) is identical in every reality.
2. **Material state** switches per reality through a Material Parameter Collection
   (`MPC_Reality`) driving material layers: restored stone/brass (Past), wet
   wear (Present), cracked ash (Future), translucent edge-glow (Echo).
   One mesh, four looks.
3. **Reality dressing sets**: actors that exist in only some realities
   (market canopies, abandoned trams, rubble, floating fragments), tagged with
   a reality mask.
4. **Lighting rigs**: each reality has its own sun/sky/fog/exposure preset,
   swapped locally on the client.
5. **VFX signatures**: dust motes and gulls (Past); rain sheets and sparks
   (Present); embers, ash and lightning (Future); filaments and floating
   motes moving upward against gravity (Echo).
6. **Audio beds** (AUDIO_DESIGN.md).

## Palette anchors
| Reality | Key | Fill | Accent |
|---|---|---|---|
| Past | Warm gold 3200 K sun | Ochre bounce | Verdigris brass |
| Present | Cold blue 9000 K night | Sodium orange practicals | Cyan screens |
| Future | Burnt orange fire | Bruised violet sky | White lightning |
| Echo | Sourceless pale cyan | Deep indigo void | Gold filaments (the only warm colour) |

## Readability rules
- Interactive objects carry a consistent, subtle rim/emissive tell in every reality.
- Each reality's interactive tell has its own shape language, not just its
  own colour, for colour-blind safety: Past = circles, Present = squares,
  Future = triangles, Echo = diamonds.
- Players' own avatars are always visible to everyone (players exist in
  every reality), each with a reality-coloured accent.

## Asset strategy
Licensed Fab/Marketplace kits and scanned materials, kitbashed and
re-materialed into an original look, plus custom hero pieces (Meridian Gate,
clock tower, MORROW ring). Every acquired asset goes in ASSET_REGISTER.md
before it is committed.
