# ftest/lib.sh - shared helpers for the functional tests (no pipes are used anywhere)

TOL=0.0011        # absolute tolerance (values are printed with 3 decimals)
RTOL=1e-9         # relative tolerance, for very large masses
TIMEOUT=120

PROG=""            # program under test: aos or soa
EXE=""             # absolute path of its executable
CASE_DIR=""        # current scratch directory (cwd of the program)
RC=0               # exit code of the last run
T_FAILS=0
T_MSGS=()

fail() { T_FAILS=$((T_FAILS + 1)); T_MSGS+=("$*"); }

new_case() {
  CASE_DIR="$(mktemp -d "$WORK/case.XXXXXX")"
  mkdir -p "$CASE_DIR/input"
}

write_input() { cat > "$CASE_DIR/input/input.txt"; }   # content comes from stdin

# run_prog <num_iterations> <width> <height> <time_step>   (any args, really)
run_prog() {
  ( cd "$CASE_DIR" && timeout "$TIMEOUT" "$EXE" "$@" >stdout.txt 2>stderr.txt )
  RC=$?
}

# short one-line summary of stderr, used inside failure messages
stderr_hint() {
  local s
  if [[ -s "$CASE_DIR/stderr.txt" ]]; then
    s="$(head -c 160 "$CASE_DIR/stderr.txt")"
    s="${s//$'\n'/ }"
    s="${s//$'\t'/ }"
    printf ' (stderr: %s)' "$s"
  fi
}

assert_rc() {  # expected exit code
  if [[ "$RC" -ne "$1" ]]; then
    fail "exit code is $RC, expected $1$(stderr_hint)"
  fi
}

assert_file() {  # file must exist; returns 1 otherwise so callers can skip
  if [[ -f "$CASE_DIR/$1" ]]; then
    return 0
  fi
  fail "$1 was not created"
  return 1
}

# number of particle lines (everything after the header)
body_lines() { awk 'END { print (NR > 0 ? NR - 1 : 0) }' "$CASE_DIR/$1"; }

# first line of a file (inside CASE_DIR)
first_line() {
  local l=""
  IFS= read -r l < "$CASE_DIR/$1"
  printf '%s' "$l"
}

# 4th field of the header line: the number of objects
header_count() {
  local a="" b="" c="" n="" rest=""
  read -r a b c n rest < "$CASE_DIR/$1"
  printf '%s' "$n"
}

assert_header() {  # file "w h dt n" (exact text)
  local got
  got="$(first_line "$1")"
  if [[ "$got" != "$2" ]]; then
    fail "$1: header is '$got', expected '$2'"
  fi
}

assert_count() {  # file n : header count field AND number of lines agree with n
  local f=$1 n=$2 hn nl
  hn="$(header_count "$f")"
  nl="$(body_lines "$f")"
  if [[ "$hn" != "$n" ]]; then
    fail "$f: header says '$hn' objects, expected $n"
  fi
  if [[ "$nl" != "$n" ]]; then
    fail "$f: has $nl particle lines, expected $n"
  fi
}

# close A B : the absolute difference of A and B is at most TOL + RTOL * the larger magnitude
close() {
  awk -v a="$1" -v b="$2" -v t="$TOL" -v r="$RTOL" 'BEGIN {
    d = a - b; if (d < 0) d = -d
    m = (a < 0 ? -a : a); n = (b < 0 ? -b : b); if (n > m) m = n
    exit !(d <= t + r * m) }'
}

# assert_particle <file> <index0> m r x y vx vy
assert_particle() {
  local f=$1 idx=$2 line i
  local tok=()
  shift 2
  local exp=("$@")
  local names=(mass radius x y vx vy)
  # the line of particle idx, without the optional "id: " prefix
  line="$(awk -v n="$((idx + 2))" 'NR == n { sub(/^[0-9]+:[ ]*/, ""); print; exit }' "$CASE_DIR/$f")"
  if [[ -z "$line" ]]; then
    fail "$f: particle #$idx is missing"
    return
  fi
  read -r -a tok <<< "$line"
  if [[ ${#tok[@]} -ne 6 ]]; then
    fail "$f: particle #$idx has ${#tok[@]} fields, expected 6 ('$line')"
    return
  fi
  for i in 0 1 2 3 4 5; do
    if ! close "${tok[$i]}" "${exp[$i]}"; then
      fail "$f: particle #$idx ${names[$i]} = ${tok[$i]}, expected ${exp[$i]}"
    fi
  done
}

# assert_format <file> : strict check of the output format from the statement
#   header: width height time_step count (3 decimals, count is an integer)
#   lines : [id: ] mass radius x y vx vy (3 decimals)
assert_format() {
  local f=$1 line first
  local header_re='^[0-9]+\.[0-9]{3} [0-9]+\.[0-9]{3} [0-9]+\.[0-9]{3} [0-9]+$'
  local line_re='^([0-9]+: )?-?[0-9]+\.[0-9]{3}( -?[0-9]+\.[0-9]{3}){5}$'
  local n=0 bad="" hn nl
  first="$(first_line "$f")"
  if [[ ! "$first" =~ $header_re ]]; then
    fail "$f: bad header '$first' (want: width height time_step count, 3 decimals)"
  fi
  while IFS= read -r line; do
    n=$((n + 1))
    if [[ $n -eq 1 ]]; then
      continue
    fi
    if [[ ! "$line" =~ $line_re ]]; then
      bad="$line"
      break
    fi
  done < "$CASE_DIR/$f"
  if [[ -n "$bad" ]]; then
    fail "$f: particle line not 'mass radius x y vx vy' with 3 decimals: $bad"
  fi
  hn="$(header_count "$f")"
  nl="$(body_lines "$f")"
  if [[ "$hn" != "$nl" ]]; then
    fail "$f: header count $hn but $nl particle lines"
  fi
}

# configs_close <fileA> <fileB> : numeric comparison of two config files
configs_close() {
  awk -v t="$TOL" -v r="$RTOL" '
    function absv(x) { return x < 0 ? -x : x }
    { sub(/^[0-9]+:[ ]*/, "") }
    FNR == 1 { file++ }
    file == 1 { A[FNR] = $0; na = FNR; next }
    { nb = FNR
      if (!(FNR in A)) { printf "line %d only in second file\n", FNR; bad = 1; exit }
      n1 = split(A[FNR], x, " "); n2 = split($0, y, " ")
      if (n1 != n2) { printf "line %d: field count differs\n", FNR; bad = 1; exit }
      for (i = 1; i <= n1; i++) {
        m = absv(x[i]); if (absv(y[i]) > m) m = absv(y[i])
        if (absv(x[i] - y[i]) > t + r * m) {
          printf "line %d field %d: %s vs %s\n", FNR, i, x[i], y[i]; bad = 1; exit } } }
    END { if (!bad && na != nb) { printf "line counts differ (%d vs %d)\n", na, nb; bad = 1 }
          exit bad }' "$1" "$2"
}


# Random input generator (deterministic for a given seed)
#   gen_random N SEED W H  ->  N valid particles, 3-decimal radii/positions

gen_random() {
  awk -v n="$1" -v seed="$2" -v w="$3" -v h="$4" 'BEGIN {
    srand(seed)
    for (i = 0; i < n; i++) {
      m  = sprintf("%.1f", 1e9 + rand() * 1e12)
      r  = sprintf("%.3f", 1 + rand() * 2) + 0
      x  = sprintf("%.3f", r + 0.001 + rand() * (w - 2 * r - 0.002))
      y  = sprintf("%.3f", r + 0.001 + rand() * (h - 2 * r - 0.002))
      vx = sprintf("%.3f", (rand() - 0.5) * 10)
      vy = sprintf("%.3f", (rand() - 0.5) * 10)
      printf "%s %.3f %s %s %s %s\n", m, r, x, y, vx, vy
    } }'
}