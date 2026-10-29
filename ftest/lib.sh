# ftest/lib.sh 

TOL=0.0011        # absolute tolerance (values are printed with 3 decimals)
RTOL=1e-9         # relative tolerance, for very large masses
TIMEOUT=120

PROG=""            # program under test: aos | soa
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

write_input() { cat > "$CASE_DIR/input/input.txt"; }   # content from stdin

# run_prog <num_iterations> <width> <height> <time_step>   (any args, really)
run_prog() {
  ( cd "$CASE_DIR" && timeout "$TIMEOUT" "$EXE" "$@" >stdout.txt 2>stderr.txt )
  RC=$?
}

stderr_hint() {
  if [[ -s "$CASE_DIR/stderr.txt" ]]; then
    printf ' (stderr: %s)' "$(head -c 160 "$CASE_DIR/stderr.txt" | tr '\n\t' '  ')"
  fi
}

assert_rc() {  # expected exit code
  [[ "$RC" -eq "$1" ]] || fail "exit code is $RC, expected $1$(stderr_hint)"
}

assert_file() {  # file must exist; returns 1 otherwise so callers can skip
  [[ -f "$CASE_DIR/$1" ]] && return 0
  fail "$1 was not created"
  return 1
}

# number of particle lines (everything after the header)
body_lines() { awk 'END { print (NR > 0 ? NR - 1 : 0) }' "$CASE_DIR/$1"; }

assert_header() {  # file "w h dt n" (exact text)
  local got; got="$(head -n1 "$CASE_DIR/$1")"
  [[ "$got" == "$2" ]] || fail "$1: header is '$got', expected '$2'"
}

assert_count() {  # file n : header count field AND number of lines agree with n
  local f=$1 n=$2 hn nl
  hn="$(head -n1 "$CASE_DIR/$f" | awk '{ print $4 }')"
  nl="$(body_lines "$f")"
  [[ "$hn" == "$n" ]] || fail "$f: header says '$hn' objects, expected $n"
  [[ "$nl" == "$n" ]] || fail "$f: has $nl particle lines, expected $n"
}

# close A B : |A-B| <= TOL + RTOL*max(|A|,|B|)
close() {
  awk -v a="$1" -v b="$2" -v t="$TOL" -v r="$RTOL" 'BEGIN {
    d = a - b; if (d < 0) d = -d
    m = (a < 0 ? -a : a); n = (b < 0 ? -b : b); if (n > m) m = n
    exit !(d <= t + r * m) }'
}

# assert_particle <file> <index0> m r x y vx vy
assert_particle() {
  local f=$1 idx=$2 line tok i
  shift 2
  local exp=("$@")
  local names=(mass radius x y vx vy)
  line="$(sed -n "$((idx + 2))p" "$CASE_DIR/$f" | sed -E 's/^[0-9]+:[[:space:]]*//')"
  if [[ -z "$line" ]]; then fail "$f: particle #$idx is missing"; return; fi
  read -r -a tok <<< "$line"
  if [[ ${#tok[@]} -ne 6 ]]; then fail "$f: particle #$idx has ${#tok[@]} fields, expected 6 ('$line')"; return; fi
  for i in 0 1 2 3 4 5; do
    close "${tok[$i]}" "${exp[$i]}" || fail "$f: particle #$idx ${names[$i]} = ${tok[$i]}, expected ${exp[$i]}"
  done
}

# assert_format <file> : strict check of the output format from the statement
assert_format() {
  local f=$1 bad
  if ! head -n1 "$CASE_DIR/$f" |
       grep -Eq '^[0-9]+\.[0-9]{3} [0-9]+\.[0-9]{3} [0-9]+\.[0-9]{3} [0-9]+$'; then
    fail "$f: bad header '$(head -n1 "$CASE_DIR/$f")' (want: width height time_step count, 3 decimals)"
  fi
  bad="$(tail -n +2 "$CASE_DIR/$f" |
         grep -nEv '^-?[0-9]+\.[0-9]{3}( -?[0-9]+\.[0-9]{3}){5}$' | head -n1)"
  [[ -z "$bad" ]] || fail "$f: particle line not 'mass radius x y vx vy' with 3 decimals: ${bad#*:}"
  local hn nl
  hn="$(head -n1 "$CASE_DIR/$f" | awk '{ print $4 }')"; nl="$(body_lines "$f")"
  [[ "$hn" == "$nl" ]] || fail "$f: header count $hn but $nl particle lines"
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