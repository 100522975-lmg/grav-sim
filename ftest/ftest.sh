#!/usr/bin/env bash
# ftest.sh - functional tests for grav-aos and grav-soa (no pipes are used anywhere)
#
# Usage: bash ftest/ftest.sh [options]
#   -b DIR   build directory (default: out/build/default, then out/build/clang-tidy)
#   -c CFG   build configuration (default: Release)
#   -p PROG  test only one program: aos or soa (default: both, plus the cross test)
#   -f REGEX run only the tests whose name matches REGEX (e.g. -f collision)
#   -k       keep the temporary work directory
#   -l       list the tests and exit
#   -h       show this help
# Environment: GRAV_AOS / GRAV_SOA = explicit path to an executable.
set -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FT_DIR="$SCRIPT_DIR"
ROOT_DIR="${FTEST_ROOT:-$(dirname "$SCRIPT_DIR")}"
BUILD_DIR="${BUILD_DIR:-}"
CONFIG="Release"
PROGS=(aos soa)
FILTER=""
KEEP=0
LIST=0

usage() {
  cat <<'EOF'
Usage: bash ftest/ftest.sh [options]
  -b DIR   build directory (default: out/build/default, then out/build/clang-tidy)
  -c CFG   build configuration (default: Release)
  -p PROG  test only one program: aos or soa (default: both, plus the cross test)
  -f REGEX run only the tests whose name matches REGEX (e.g. -f collision)
  -k       keep the temporary work directory
  -l       list the tests and exit
  -h       show this help
Environment: GRAV_AOS / GRAV_SOA = explicit path to an executable.
EOF
}

while getopts "b:c:p:f:klh" opt; do
  case "$opt" in
    b) BUILD_DIR="$OPTARG" ;;
    c) CONFIG="$OPTARG" ;;
    p) PROGS=("$OPTARG") ;;
    f) FILTER="$OPTARG" ;;
    k) KEEP=1 ;;
    l) LIST=1 ;;
    h) usage; exit 0 ;;
    *) usage; exit 2 ;;
  esac
done
for p in "${PROGS[@]}"; do
  if [[ "$p" != aos && "$p" != soa ]]; then
    echo "ftest: unknown program '$p' (use aos or soa)" >&2
    exit 2
  fi
done

if [[ -t 1 ]]; then
  GREEN=$'\033[32m'; RED=$'\033[31m'; YEL=$'\033[33m'; OFF=$'\033[0m'
else
  GREEN=""; RED=""; YEL=""; OFF=""
fi


# Load framework and tests

if [[ ! -f "$FT_DIR/lib.sh" ]]; then
  echo "ftest: missing $FT_DIR/lib.sh" >&2
  exit 2
fi
# shellcheck source=/dev/null
source "$FT_DIR/lib.sh"
for f in "$FT_DIR"/test_*.sh; do
  # shellcheck source=/dev/null
  source "$f"
done

# per-program tests are the functions named t_*, cross tests are named x_*
mapfile -t PER_PROG_TESTS < <(compgen -A function t_)
mapfile -t CROSS_TESTS < <(compgen -A function x_)

if [[ $LIST -eq 1 ]]; then
  printf '%s\n' "${PER_PROG_TESTS[@]}" "${CROSS_TESTS[@]}"
  exit 0
fi


# Locating executables

find_exe() {  # $1 = aos or soa  -> prints absolute path (or nothing)
  local name="$1" var="GRAV_${1^^}" d cand
  if [[ -n "${!var:-}" ]]; then
    if [[ -x "${!var}" ]]; then
      realpath "${!var}"
    fi
    return 0
  fi
  local dirs=()
  if [[ -n "$BUILD_DIR" ]]; then
    dirs+=("$BUILD_DIR")
  fi
  dirs+=("$ROOT_DIR/out/build/default" "$ROOT_DIR/out/build/clang-tidy")
  for d in "${dirs[@]}"; do
    cand="$d/$name/$CONFIG/grav-$name"
    if [[ -x "$cand" ]]; then
      realpath "$cand"
      return 0
    fi
  done
  return 0
}

declare -A EXE_OF
for p in "${PROGS[@]}"; do
  EXE_OF[$p]="$(find_exe "$p")"
  if [[ -z "${EXE_OF[$p]}" ]]; then
    echo "ftest: cannot find grav-$p (looked for <build>/$p/$CONFIG/grav-$p)." >&2
    echo "       Build it first (e.g. 'cmake --preset default && cmake --build --preset release')" >&2
    echo "       or pass -b DIR / set GRAV_${p^^}=/path/to/grav-$p." >&2
    exit 2
  fi
done
WORK="$(mktemp -d "${TMPDIR:-/tmp}/ftest.XXXXXX")"
if [[ $KEEP -eq 0 ]]; then
  trap 'rm -rf "$WORK"' EXIT
fi


# Runner

PASSED=0; FAILED=0; FAILED_NAMES=()

run_one() {  # <display name> <function>
  local name=$1 fn=$2 m
  if [[ -n "$FILTER" && ! "$name" =~ $FILTER ]]; then
    return
  fi
  T_FAILS=0; T_MSGS=()
  "$fn"
  if (( T_FAILS == 0 )); then
    PASSED=$((PASSED + 1))
    printf '%sPASS%s  %s\n' "$GREEN" "$OFF" "$name"
  else
    FAILED=$((FAILED + 1)); FAILED_NAMES+=("$name")
    printf '%sFAIL%s  %s\n' "$RED" "$OFF" "$name"
    for m in "${T_MSGS[@]}"; do
      printf '        - %s\n' "$m"
    done
  fi
}

for PROG in "${PROGS[@]}"; do
  EXE="${EXE_OF[$PROG]}"
  echo "=== grav-$PROG  ($EXE)"
  for t in "${PER_PROG_TESTS[@]}"; do
    run_one "$PROG/${t#t_}" "$t"
  done
done
if [[ ${#PROGS[@]} -eq 2 ]]; then
  echo "=== aos vs soa"
  for t in "${CROSS_TESTS[@]}"; do
    run_one "cross/${t#x_}" "$t"
  done
fi

echo
printf 'Result: %s%d passed%s, %s%d failed%s\n' "$GREEN" "$PASSED" "$OFF" "$RED" "$FAILED" "$OFF"
if (( FAILED > 0 )); then
  printf '%sFailed:%s %s\n' "$YEL" "$OFF" "${FAILED_NAMES[*]}"
  if [[ $KEEP -eq 1 ]]; then
    echo "Work directory kept: $WORK"
  fi
  exit 1
fi
exit 0