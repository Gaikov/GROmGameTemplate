#!/usr/bin/env bash

set -euo pipefail

readonly ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly EMSDK_DIR="${EMSDK:-$HOME/emsdk}"
readonly EMSCRIPTEN_VERSION="6.0.4"

if [[ ! -f "$EMSDK_DIR/emsdk_env.sh" ]]; then
	echo "Emscripten SDK not found at $EMSDK_DIR" >&2
	echo "Install emsdk $EMSCRIPTEN_VERSION or set EMSDK to its directory." >&2
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

cmake -E rm -rf "$ROOT_DIR/bin"
emcmake cmake \
	-S "$ROOT_DIR" \
	-B "$ROOT_DIR/bin" \
	-G Ninja \
	-DWEB_ASM=ON \
	-DCMAKE_BUILD_TYPE=Release

"$ROOT_DIR/wasm-build.sh"
