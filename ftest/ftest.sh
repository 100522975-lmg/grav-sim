#!/usr/bin/env bash
set -u
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FT_DIR="$SCRIPT_DIR"
ROOT_DIR="${FTEST_ROOT:-$(dirname "$SCRIPT_DIR")}"
BUILD_DIR="${BUILD_DIR:-}"
CONFIG="Release"
PROGS=(aos soa)
FILTER=""
KEEP=0
LIST=0

usage() { sed -n '2,/^# ------.*$/p' "$0" | sed -n '2,26p' | sed 's/^# \{0,1\}//'; }

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
  [[ "$p" == aos || "$p" == soa ]] || { echo "ftest: unknown program '$p' (use aos or soa)" >&2; exit 2; }
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

mapfile -t PER_PROG_TESTS < <(compgen -A function | grep '^t_' | sort)
mapfile -t CROSS_TESTS    < <(compgen -A function | grep '^x_' | sort)

if [[ $LIST -eq 1 ]]; then
  printf '%s\n' "${PER_PROG_TESTS[@]}" "${CROSS_TESTS[@]}"
  exit 0
fi


# Locating executables

find_exe() {  # $1 = aos | soa  -> prints absolute path (or nothing)
  local name="$1" var="GRAV_${1^^}" d cand
  if [[ -n "${!var:-}" ]]; then
    [[ -x "${!var}" ]] && realpath "${!var}"
    return 0
  fi
  local dirs=()
  [[ -n "$BUILD_DIR" ]] && dirs+=("$BUILD_DIR")
  dirs+=("$SCRIPT_DIR/out/build/default" "$SCRIPT_DIR/out/build/clang-tidy")
  for d in "${dirs[@]}"; do
    cand="$d/$name/$CONFIG/grav-$name"
    if [[ -x "$cand" ]]; then realpath "$cand"; return 0; fi
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
if [[ $KEEP -eq 0 ]]; then trap 'rm -rf "$WORK"' EXIT; fi


# Runner

PASSED=0; FAILED=0; FAILED_NAMES=()

run_one() {  # <display name> <function>
  local name=$1 fn=$2
  if [[ -n "$FILTER" && ! "$name" =~ $FILTER ]]; then return; fi
  T_FAILS=0; T_MSGS=()
  "$fn"
  if (( T_FAILS == 0 )); then
    PASSED=$((PASSED + 1))
    printf '%sPASS%s  %s\n' "$GREEN" "$OFF" "$name"
  else
    FAILED=$((FAILED + 1)); FAILED_NAMES+=("$name")
    printf '%sFAIL%s  %s\n' "$RED" "$OFF" "$name"
    local m
    for m in "${T_MSGS[@]}"; do printf '        - %s\n' "$m"; done
  fi
}

for PROG in "${PROGS[@]}"; do
  EXE="${EXE_OF[$PROG]}"
  echo "=== grav-$PROG  ($EXE)"
  for t in "${PER_PROG_TESTS[@]}"; do run_one "$PROG/${t#t_}" "$t"; done
done
if [[ ${#PROGS[@]} -eq 2 ]]; then
  echo "=== aos vs soa"
  for t in "${CROSS_TESTS[@]}"; do run_one "cross/${t#x_}" "$t"; done
fi

echo
printf 'Result: %s%d passed%s, %s%d failed%s\n' "$GREEN" "$PASSED" "$OFF" "$RED" "$FAILED" "$OFF"
if (( FAILED > 0 )); then
  printf '%sFailed:%s %s\n' "$YEL" "$OFF" "${FAILED_NAMES[*]}"
  [[ $KEEP -eq 1 ]] && echo "Work directory kept: $WORK"
  exit 1
fi
exit 0