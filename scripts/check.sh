#!/usr/bin/env bash
# The one check every change must pass. Prints "check: PASS" at the end on success.
# Usage: scripts/check.sh
set -euo pipefail
cd "$(dirname "$0")/.."

fail() { echo "check: FAIL - $*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || fail "$1 is not installed (see docs/BUILDING.md)"; }

if [ -x .venv/bin/python ]; then PYTHON=.venv/bin/python; else PYTHON=python3; fi

echo "check: 1/6 shellcheck"
need shellcheck
shellcheck -x scripts/*.sh

echo "check: 2/6 no game data or binaries tracked by git"
bad="$(git ls-files | grep -Ei '\.(png|jpe?g|webp|ogg|opus|mp3|wav|flac|at3|webm|mkv|mp4|pmf|rpa|rpyc|kpk|ksb|pbp|prx|elf|ttf|otf)$' || true)"
[ -z "$bad" ] || fail "these files must not be committed:
$bad"

echo "check: 3/6 C formatting"
need clang-format
c_files="$(git ls-files --cached --others --exclude-standard 'src/*.c' 'src/*.h' 'tests/c/*.c' 'tests/c/*.h')"
if [ -n "$c_files" ]; then
  # shellcheck disable=SC2086
  clang-format --dry-run --Werror $c_files || fail "run: scripts/format.sh"
fi

echo "check: 4/6 host build"
need cmake
mkdir -p build
cmake -S . -B build/host -DCMAKE_BUILD_TYPE=Debug -DKS_BUILD_APP="${KS_BUILD_APP:-ON}" >build/host.configure.log 2>&1 \
  || { cat build/host.configure.log; fail "cmake configure failed"; }
if ! cmake --build build/host --parallel >build/host.build.log 2>&1; then
  grep -E -A3 'error' build/host.build.log || tail -n 30 build/host.build.log
  fail "host build failed (full log: build/host.build.log)"
fi
if grep -E 'warning:' build/host.build.log; then
  fail "compiler warnings (full log: build/host.build.log)"
fi

echo "check: 5/6 C tests"
ctest --test-dir build/host --output-on-failure || fail "C tests failed"

echo "check: 6/6 Python tests"
"$PYTHON" -m pytest -q || fail "Python tests failed"

echo "check: PASS"
