#!/usr/bin/env bash
# Build the engine.
# Usage: scripts/build.sh [host|psp]
#   host  desktop build in build/host (default)
#   psp   EBOOT.PBP in dist/PSP/GAME/<MS_NAME>/, needs the pspdev toolchain
set -euo pipefail
cd "$(dirname "$0")/.."
# shellcheck source=versions.env
source versions.env

target="${1:-host}"
case "$target" in
  host)
    cmake -S . -B build/host -DCMAKE_BUILD_TYPE=Release
    cmake --build build/host --parallel
    echo "build: done, run build/host/ks"
    ;;
  psp)
    if ! command -v psp-cmake >/dev/null 2>&1; then
      if [ -x "${PSPDEV:-$HOME/pspdev}/bin/psp-cmake" ]; then
        export PSPDEV="${PSPDEV:-$HOME/pspdev}"
        export PATH="$PSPDEV/bin:$PATH"
      else
        echo "build: ERROR psp-cmake not found, install pspdev (see docs/BUILDING.md)" >&2
        exit 1
      fi
    fi
    psp-cmake -S . -B build/psp -DCMAKE_BUILD_TYPE=Release -DKS_BUILD_TESTS=OFF
    cmake --build build/psp --parallel
    out="dist/PSP/GAME/$MS_NAME"
    mkdir -p "$out"
    cp build/psp/EBOOT.PBP "$out/EBOOT.PBP"
    echo "build: done, $out/EBOOT.PBP"
    ;;
  *)
    echo "usage: scripts/build.sh [host|psp]" >&2
    exit 2
    ;;
esac
