#!/usr/bin/env bash
set -eu

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$repo_dir"

if command -v python3 >/dev/null 2>&1; then
    exec python3 script/build.py --language auto "$@"
fi

if command -v python >/dev/null 2>&1; then
    exec python script/build.py --language auto "$@"
fi

printf '%s\n' '[ERROR] Python 3 was not found.' >&2
printf '%s\n' 'Install Python 3 (e.g. brew install python), then run build_mac.sh again.' >&2
exit 1
