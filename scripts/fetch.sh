#!/usr/bin/env bash
# Download the pinned game source into upstream/ (git-ignored). About 1.7 GB.
# Usage: scripts/fetch.sh
set -euo pipefail
cd "$(dirname "$0")/.."
# shellcheck source=versions.env
source versions.env

mkdir -p upstream
if [ ! -d upstream/ksre/.git ]; then
  echo "fetch: cloning KS:RE $KSRE_TAG (about 1.7 GB, be patient)"
  git clone --depth 1 --branch "$KSRE_TAG" "$KSRE_REPO" upstream/ksre
fi
got="$(git -C upstream/ksre rev-parse HEAD)"
if [ "$got" != "$KSRE_COMMIT" ]; then
  echo "fetch: ERROR upstream/ksre is at $got, expected $KSRE_COMMIT" >&2
  echo "fetch: delete upstream/ksre and run this script again" >&2
  exit 1
fi
echo "fetch: KS:RE ok ($KSRE_TAG)"
echo "fetch: done"
