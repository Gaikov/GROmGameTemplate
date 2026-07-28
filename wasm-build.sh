#!/usr/bin/env bash

set -euo pipefail

readonly ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly EMSDK_DIR="${EMSDK:-$HOME/emsdk}"
readonly EMSCRIPTEN_VERSION="6.0.4"

if [[ ! -f "$EMSDK_DIR/emsdk_env.sh" ]]; then
	echo "Emscripten SDK not found at $EMSDK_DIR" >&2
	exit 1
fi

export EMSDK_QUIET=1
source "$EMSDK_DIR/emsdk_env.sh"

for tool in emcc em++ emar emcmake; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		echo "Required Emscripten tool not found: $tool" >&2
		exit 1
	fi
done

if [[ "$(emcc --version | head -n 1)" != *" $EMSCRIPTEN_VERSION "* ]]; then
	echo "Emscripten $EMSCRIPTEN_VERSION is required." >&2
	emcc --version | head -n 1 >&2
	exit 1
fi

if [[ ! -f "$ROOT_DIR/bin/CMakeCache.txt" ]]; then
	echo "WASM build is not configured. Run ./wasm-prepare.sh first." >&2
	exit 1
fi

cmake --build "$ROOT_DIR/bin" --parallel
