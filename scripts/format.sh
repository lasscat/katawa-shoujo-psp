#!/usr/bin/env bash
# Reformat all C sources in place with clang-format.
# Usage: scripts/format.sh
set -euo pipefail
cd "$(dirname "$0")/.."

git ls-files --cached --others --exclude-standard 'src/*.c' 'src/*.h' 'tests/c/*.c' 'tests/c/*.h' \
  | xargs clang-format -i
echo "format: done"
