#!/usr/bin/env bash
set -euo pipefail
if [[ $# -ne 1 || -z "$1" ]]; then
  echo "Usage: $0 SERIAL_PORT" >&2
  exit 2
fi
project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
"$project_root/scripts/build.sh"
arduino-cli upload --port "$1" \
  --fqbn esp32:esp32:esp32c3:CDCOnBoot=cdc \
  --input-dir "$project_root/build" \
  "$project_root/firmware/sv48p-esp32c3-moonlite-focuser"
