#!/usr/bin/env sh
set -u

repo_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$repo_dir" || exit 1

if command -v python3 >/dev/null 2>&1; then
    exec python3 script/build.py "$@"
fi

if command -v python >/dev/null 2>&1; then
    exec python script/build.py "$@"
fi

printf '%s\n' '[ERROR] Python 3 was not found.' >&2
exit 1
