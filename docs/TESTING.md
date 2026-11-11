# Testing

## Automated

`scripts/check.sh` runs everything and must end with `check: PASS`:

1. `shellcheck` on `scripts/`
2. no game data or binaries tracked by git
3. C formatting (`scripts/format.sh` fixes it)
4. host build with warnings treated as failures
5. C unit tests (`ctest`)
6. Python unit tests (`pytest`)

Run one C test: `build/host/test_crc32`. Run one Python test file: `.venv/bin/pytest tests/py/test_cli.py`.

### Writing a C test

Create `tests/c/test_<module>.c`. It is picked up automatically.

```c
#include "ks_test.h"
#include "<module>.h"

int main(void) {
    CHECK_EQ_INT(1 + 1, 2);
    return ks_test_result();
}
```

Tests must not read files from `upstream/`. Build any input in memory or in a temporary file.

## Manual

- Desktop: `build/host/ks`. `build/host/ks --frames 60` renders 60 frames and exits.
- Emulator: `scripts/build.sh psp && scripts/run-ppsspp.sh`.
- Hardware: copy `dist/PSP/` to the memory stick. Changes to rendering, audio, input, memory use or
  loading must be tried on a real PSP before release; note the model (1000/2000/3000/Go/Street).
