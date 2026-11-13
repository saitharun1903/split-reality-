# Phase 0 — Environment & Repository Audit

Audit date: 2026-11-13. Every value below was measured by running commands in
this environment; nothing was assumed.

## 1. Environment report (cloud development container)

| Item | Result | Consequence |
|---|---|---|
| OS | Ubuntu 24.04.5 LTS, x86_64, Linux 6.18 | Linux only; no Windows toolchain |
| CPU / RAM | 4 vCPU / 15 GiB, no swap | Far below what an Unreal source build or editor needs |
| Free disk | ~31 GB writable allowance | A UE5 source checkout plus build needs well over 100 GB; impossible here |
| GPU | **None** (no `/dev/dri`, no `nvidia-smi`) | No rendering, no editor, no shader compilation, no visual verification |
| Unreal Engine | **Not installed** (searched `/opt`, `/usr/local`, `$HOME`, `/` depth 4) | No UE build, cook, package or test can run here |
| Epic source / images | `api.github.com/repos/EpicGames/UnrealEngine` → 403; `ghcr.io/epicgames` → 401 | Engine source requires an Epic-linked GitHub account; not reachable from here |
| C++ compilers | GCC 13.3, Clang 18.1.3 | Engine-independent C++ can be built and tested |
| Build tools | CMake 3.28, Ninja 1.11, Make 4.3 | Used for the SRCore test build |
| .NET / Mono | Missing | UnrealBuildTool cannot run here |
| Android SDK/NDK, adb | Missing (`ANDROID_HOME` unset) | No Android builds or device tests |
| Java | Present (OpenJDK) | Not useful without the Android SDK |
| Node.js / npm | Node 22, npm 10 | Viable for future backend/web prototypes |
| Python | 3.13 | Tooling scripts |
| Git / Git LFS | 2.43 / 3.4.1 | Repository and LFS ready |
| Docker CLI | 29.9 (CLI present; daemon not verified) | Possible later for backend services |
| Clang sanitizers | ASan runtime missing for Clang; GCC ASan/UBSan works | Sanitizer runs use GCC |

**Conclusion:** this container can do design, documentation, engine-independent
C++ (compiled and unit-tested), backend/web prototyping and repository
management. It **cannot** run Unreal Engine in any form. All Unreal work
(editor, C++ modules, Blueprints, levels, cinematics, packaging, profiling)
must happen on a workstation with UE5 and a GPU.

## 2. Repository report

- The working directory was empty: no files, no git history, no CLAUDE.md.
- The user's GitHub account has no existing SPLIT REALITY repository.
- Nothing needed preserving. A new repository was initialised at
  `SPLIT-REALITY/` on branch `main`.

## 3. Technical risk report (summary; full list in DEVELOPMENT_STATUS.md)

1. **No Unreal-capable machine has been identified.** This blocks Phases 2–9.
2. **Visual ambition vs team size.** "Realistic AAA" art needs artists,
   licensed assets and time. AI-written code does not create it.
3. **Android.** The four-reality lighting concept collides with mobile
   rendering limits (no Lumen or Nanite on typical Android devices).
4. **Web.** Unreal has no supported WebAssembly export. Every web option
   either costs GPU-hours per player (Pixel Streaming) or means a second client.
5. **Listen-server limits.** If the host leaves, the session ends unless a
   dedicated server is used, and dedicated servers need a source-built engine.

## 4. Recommended project configuration (for the UE workstation)

| Setting | Recommendation | Why |
|---|---|---|
| Engine | Latest stable UE 5.x hotfix from the Epic Launcher, pinned in `DECISIONS.md` the day it is installed | Launcher builds are enough for the slice (listen server); move to a source build only when dedicated servers are needed |
| Workstation | Windows 10/11, 8+ cores, 32–64 GB RAM, DX12 GPU with ≥ 8 GB VRAM, ≥ 500 GB SSD | Epic's editor guidance plus headroom for shader compilation and DDC |
| IDE | Visual Studio 2022 (Game development with C++) or JetBrains Rider | Supported UE toolchains |
| Project | C++ Third Person template, renamed `SplitReality`, in `Game/SplitReality/` | Ships with Enhanced Input and a tuned character |
| Version control | Git + Git LFS (`.gitattributes` already in place) | Already configured; Perforce only if the team grows |
| Online (slice) | `OnlineSubsystemNull` with LAN/direct IP, listen server | Smallest infrastructure that proves the game |
| Online (after slice) | Epic Online Services (EOS) for lobbies, P2P relay and identity | Free and cross-platform; covers PC and Android |
