#!/usr/bin/env bash
set -euo pipefail

DEFAULT_ISL_URL="https://github.com/Meinersbur/isl/archive/refs/tags/isl-0.24.tar.gz"

url="${1:-${ISL_URL:-$DEFAULT_ISL_URL}}"
output_dir="${2:-${ISL_OUTPUT_DIR:-downloads}}"

mkdir -p "$output_dir"

filename="${url##*/}"
output_path="$output_dir/$filename"

if command -v curl >/dev/null 2>&1; then
  curl -fL "$url" -o "$output_path"
elif command -v wget >/dev/null 2>&1; then
  wget -O "$output_path" "$url"
else
  echo "Error: curl or wget is required to download ISL." >&2
  exit 1
fi

echo "Downloaded ISL to $output_path"
