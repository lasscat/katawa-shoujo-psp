#!/usr/bin/env bash
# Start the PSP build in the PPSSPP emulator.
# Usage: scripts/run-ppsspp.sh
set -euo pipefail
cd "$(dirname "$0")/.."

# PPSSPP remaps paths that contain PSP/GAME to its own memory stick folder, so run the copy in build/.
eboot="build/psp/EBOOT.PBP"
[ -f "$eboot" ] || { echo "run-ppsspp: ERROR $eboot not found, run scripts/build.sh psp" >&2; exit 1; }

for candidate in PPSSPPSDL ppsspp PPSSPPQt; do
  if command -v "$candidate" >/dev/null 2>&1; then
    exec "$candidate" "$PWD/$eboot"
  fi
done
echo "run-ppsspp: ERROR PPSSPP not found in PATH" >&2
exit 1
