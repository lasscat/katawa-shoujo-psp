# Katawa Shoujo for PSP

A native PlayStation Portable port of the visual novel *Katawa Shoujo*, built on the scripts and
remastered assets of [Katawa Shoujo: Re-Engineered](https://codeberg.org/fhs/katawa-shoujo-re-engineered).

The original game runs on Ren'Py, which needs far more memory and CPU than a PSP has. This project
takes a different route:

- a small visual novel **engine** written in C99 for the PSP (and for desktop, where it is tested), and
- a **converter** that runs on your computer and turns the game's Ren'Py scripts, images and audio
  into compact data the engine can load.

**Status: early development. Not playable yet.** See [docs/STATUS.md](docs/STATUS.md).

This repository contains no game assets. You build the data yourself from the upstream game; see
[NOTICE](NOTICE).

## Quick start

```bash
scripts/build.sh host      # desktop build      -> build/host/ks
scripts/build.sh psp       # PSP build          -> dist/PSP/GAME/ks/EBOOT.PBP
scripts/run-ppsspp.sh      # run the PSP build in PPSSPP
scripts/check.sh           # formatting, build and all tests; must end with "check: PASS"
```

Requirements and setup are in [docs/BUILDING.md](docs/BUILDING.md).

## Layout

| Path | Contents |
|---|---|
| `src/` | Engine. Portable core plus a thin SDL2 front end shared by PSP and desktop |
| `tools/kstool/` | Converter: script compiler, asset pipeline, pack writer (Python) |
| `tests/` | Unit tests for the engine (`tests/c`) and the converter (`tests/py`) |
| `scripts/` | Build, check and fetch scripts |
| `docs/` | [Design](docs/DESIGN.md), [file formats](docs/FORMATS.md), [building](docs/BUILDING.md), [testing](docs/TESTING.md) |

## Contributing

Bug reports and patches are welcome. Please read [CONTRIBUTING.md](CONTRIBUTING.md) first.

## Licence and credits

Code in this repository is licensed under the [Mozilla Public License 2.0](LICENSE).

*Katawa Shoujo* is by Four Leaf Studios. *Katawa Shoujo: Re-Engineered* is by Fleeting Heartbeat
Studios. This project is not affiliated with or endorsed by either, or by Sony.
Katawa Shoujo contains adult content and is intended for players 18 and over.
