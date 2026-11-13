# Platform Strategy

Three separate workstreams, each validated separately. The PC architecture
is never compromised to pretend one build targets everything.

## PC (Windows): lead platform
Full-quality experience: Lumen/Nanite on High/Epic, scalability tiers down
to a Low tier without Lumen, keyboard/mouse and controller, EOS (Steam
distribution optional later). **Deliverable: yes**, once a UE workstation exists.

## Android: dedicated adaptation
What is genuinely achievable: a version of Chapter 1 on **recent high-end
Android devices** (device floor to be fixed after profiling). It needs:
- Mobile renderer (Vulkan); **no Lumen or Nanite** on typical Android hardware;
  hand-authored LODs; mobile-specific materials (a cheaper `MPC_Reality` path).
- **Per-reality lighting** without Lumen: evaluate *Lighting Scenarios*
  (one baked scenario per reality, switched locally on phase) against
  stationary lights with per-reality presets. This is the biggest technical
  unknown on Android (spike S4).
- Touch controls, safe-area HUD, 30 fps target, thermal testing.
- Toolchain: Android Studio + the SDK/NDK versions required by the pinned UE version, on the workstation.

**Recommendation:** start Android only after the PC slice is accepted, with a
2-week spike to decide between a scaled build and a visually "adapted" build.
Cross-play with PC is possible through EOS but adds QA cost; decide after the spike.

## Web: options compared
Unreal Engine has **no supported WebAssembly/HTML5 export** for UE5 projects.
Two real options:

| | Pixel Streaming (UE renders in the cloud, video to the browser) | Browser-native client (e.g. Three.js / Babylon.js) |
|---|---|---|
| Visual quality | Identical to PC | Much lower; separate art pipeline |
| Engineering cost | Low to moderate (hosting, signalling, matchmaking of GPU instances) | Very high: a second game client, re-implemented gameplay and netcode (SRCore could be compiled to WASM to share rules) |
| Running cost | **One cloud GPU per concurrent player**, roughly $0.5–1+ per player-hour at public on-demand GPU prices (verify current pricing); a 4-player session is 4 streams | Ordinary web hosting; near zero per player |
| Latency | Input-to-photon includes the network round trip; needs a nearby region (< ~50 ms RTT) for comfort | Local rendering; only game netcode latency |
| GPU requirement (player) | None. Any modern browser with WebRTC | WebGL2/WebGPU-capable device |
| Browser support | Chromium, Firefox and Safari via WebRTC; mobile browsers work but bandwidth heavy (~5–15 Mbps) | WebGL2 broadly; WebGPU growing |
| Multiplayer sync | Trivial: the streamed instances are normal game clients | Must speak the UE protocol (impractical) or a custom protocol via a separate backend |
| Scalability | Linear cost; capacity planning for GPU pools | Scales like a website |
| UX | Feels like the real game; queueing if capacity is full | Feels like a different, smaller game |

**Recommendation:** treat Web as **Pixel Streaming for limited access**
(demos, press, "try before you download", invite-only sessions), using the
same game build. **Do not build a browser-native client** unless the owner
decides a permanent free web version is a business goal, because that is a second game.
Web work starts only after the PC slice is accepted. Decision D-05 needs owner approval.
