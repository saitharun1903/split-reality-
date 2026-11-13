# Performance Budget

**These are targets, not measurements.** No benchmark has been run: no UE
build exists yet. Hardware classes will be replaced by named test machines
once they exist. Every measurement recorded here must include machine,
build configuration, map and date.

## PC tiers (1080p unless noted)
| Tier | Reference class | Target | Frame | Key settings |
|---|---|---|---|---|
| Epic | RTX 4070-class | 60 fps @ 1440p (TSR) | 16.6 ms | Lumen HW RT where available, Nanite, VSM |
| High (**slice gate**) | RTX 3060 / RX 6600-class | 60 fps @ 1080p (TSR) | 16.6 ms | Lumen SW, Nanite, VSM |
| Low | GTX 1660-class | 30–60 fps @ 1080p | 33 ms | Lumen off (screen-space GI + sky light), shadow maps, reduced VFX |

High-tier frame split target: game thread ≤ 10 ms, render thread ≤ 10 ms, GPU ≤ 14 ms.
Memory: process ≤ 8 GB RAM, VRAM ≤ 6 GB at High.
Loading: main menu → playable Chapter 1 ≤ 20 s from SSD.
Shader compilation: precompiled PSO cache shipped; no hitch > 50 ms during the first playthrough after warm-up.

## Android (provisional, set after spike S4)
30 fps (33 ms) on the device floor; ≤ ~700 draw calls and ≤ ~1 M visible
triangles; texture memory ≤ 1 GB; no sustained thermal throttling over a 20-minute session.

## Network
≤ 20 KB/s per client steady state; phase and puzzle events ≤ 100 ms server
processing; correct behaviour at 150 ms RTT + 2 % loss.

## Profiling tools (on the UE workstation)
Unreal Insights (CPU/GPU/memory/network/loading), `stat unit`/`stat gpu`,
GPU Visualizer, Network Profiler / Insights networking, Android GPU
Inspector / Snapdragon Profiler / Arm Performance Studio for Android.
