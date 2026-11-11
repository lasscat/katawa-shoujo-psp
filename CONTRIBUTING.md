# Contributing

## Before you start

- For anything larger than a small fix, open an issue first so the approach can be agreed on.
- Read [docs/DESIGN.md](docs/DESIGN.md). It explains what belongs in the engine and what belongs
  in the converter.

## Rules

1. `scripts/check.sh` must end with `check: PASS`. CI runs the same script.
2. Never commit game data: no images, audio, video, fonts, `.rpa`, `.kpk`, `.ksb` or built binaries.
   Tests use small synthetic fixtures generated in the test itself.
3. Engine code in `src/` outside `src/main.c` is portable C99: no SDL and no PSP headers. That is
   what keeps it unit-testable on a desktop.
4. Every new engine module `src/<name>.c` comes with `tests/c/test_<name>.c`. Every new converter
   module comes with a `tests/py/test_<name>.py`.
5. No dynamic allocation in per-frame code. Allocate when a scene loads, not while it runs.
6. One logical change per pull request. Keep them small.

## Style

- C: formatted by `scripts/format.sh` (clang-format). Public names start with `ks_`. Fixed-width
  integer types for anything that is stored in a file.
- Python: standard library only in `tools/kstool` unless an issue agrees otherwise; type hints on
  public functions; must run on Python 3.9.
- Shell: bash, `set -euo pipefail`, passes `shellcheck`.

## Commit messages

Imperative summary line under 72 characters, for example `Add pack index lookup`. Reference the
issue in the pull request, not in every commit.
