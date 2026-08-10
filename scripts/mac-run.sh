#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="$repo_root/build/guiapp-macos"

cmake -S "$repo_root/projects/guiapp/macos" -B "$build_dir"
cmake --build "$build_dir"
exec "$build_dir/guiapp-macos" "$@"
