#!/bin/bash
set -euo pipefail

# Build script for Pikranji NDS port
# Uses devkitPro Docker image for compilation
# Reference: https://github.com/devkitPro/docker

IMAGE_NAME="devkitpro/devkitarm"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# If "clean" argument passed, clean build directory first
if [ "${1:-}" == "clean" ]; then
    echo "🧹 Cleaning previous builds..."
    rm -rf "$SCRIPT_DIR/build"
    docker run --rm -v "$SCRIPT_DIR":/source -w /source "$IMAGE_NAME" make clean
fi

echo "🚀 Compiling Pikranji..."
# Compile using devkitPro Docker image
docker run --rm -v "$SCRIPT_DIR":/source -w /source "$IMAGE_NAME" make

# The Makefile TARGET is derived from the directory name ("pikranji"),
# so the output is always pikranji.nds — never source.nds.
if [ -f "$SCRIPT_DIR/pikranji.nds" ]; then
    echo "✅ Success! 'pikranji.nds' created."
else
    echo "❌ Compilation failed — pikranji.nds not found."
    exit 1
fi