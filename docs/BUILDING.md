# Building

## Requirements

| Tool | Used for | macOS (Homebrew) | Debian / Ubuntu |
|---|---|---|---|
| CMake 3.16+, a C compiler | host build | `brew install cmake` | `apt install cmake build-essential` |
| SDL2 | host build | `brew install sdl2` | `apt install libsdl2-dev` |
| clang-format, shellcheck | `scripts/check.sh` | `brew install clang-format shellcheck` | `apt install clang-format shellcheck` |
| Python 3.9+ and pytest | converter and its tests | see below | see below |
| [pspdev](https://pspdev.github.io/) | PSP build | see below | see below |
| [PPSSPP](https://www.ppsspp.org/) | running the PSP build | `brew install ppsspp` | `apt install ppsspp` |

Python environment:

```bash
python3 -m venv .venv
.venv/bin/pip install -r requirements-dev.txt
```

PSP toolchain: download the archive for your system from the
[pspdev releases](https://github.com/pspdev/pspdev/releases), unpack it to `~/pspdev`, and add this
to your shell profile:

```bash
export PSPDEV="$HOME/pspdev"
export PATH="$PATH:$PSPDEV/bin"
```

On macOS the toolchain also needs `brew install gmp mpfr libmpc isl zstd`.

## Host build

```bash
scripts/build.sh host
build/host/ks
```

A 480x272 window opens. Escape quits.

## PSP build

```bash
scripts/build.sh psp
scripts/run-ppsspp.sh
```

For real hardware (custom firmware required), copy `dist/PSP/` to the root of the memory stick.

## Game data

```bash
scripts/fetch.sh
```

downloads the pinned upstream game into `upstream/ksre/` (about 1.7 GB). Converting it into PSP
data is not implemented yet; see [STATUS.md](STATUS.md).
