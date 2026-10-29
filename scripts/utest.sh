#!/usr/bin/env bash
# utest.sh - Builds the project and runs ALL unit tests using clang-tidy  in Release mode (utcommon, utaos and utsoa).
# The exit code is 0 only if compilation and all tests pass.

set -eu

# Navigate to the repository root directory
cd "$(dirname "$0")/.."

# Check required tools.
for tool in cmake ctest; do
  if ! command -v "$tool" > /dev/null 2>&1; then
    echo "utest.sh: '$tool' not found. Please install CMake." >&2
    exit 1
  fi
done

echo "==> Configuring with the clang-tidy preset"
cmake --preset clang-tidy

echo "==> Building all targets in Release mode"
cmake --build out/build/clang-tidy --config Release

echo "==> Running all unit tests in Release mode"
ctest --test-dir out/build/clang-tidy \
  -C Release \
  --output-on-failure \
  "$@"

echo "==> All unit tests passed!"