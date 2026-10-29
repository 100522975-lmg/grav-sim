# ftest/test_random.sh - larger randomised scenarios and the AOS/SOA cross test

RANDOM_N=300; RANDOM_SEED=42; RANDOM_W=150; RANDOM_H=150

t_random_invariants() {
  new_case
  gen_random "$RANDOM_N" "$RANDOM_SEED" "$RANDOM_W" "$RANDOM_H" > "$CASE_DIR/input/input.txt"
  run_prog 5 "$RANDOM_W" "$RANDOM_H" 0.25
  assert_rc 0
  assert_file init_config.txt  || return
  assert_file final_config.txt || return
  assert_format init_config.txt
  assert_format final_config.txt
  local in_mass init_n final_n stats f
  in_mass="$(awk '{ s += $1 } END { printf "%.6f", s }' "$CASE_DIR/input/input.txt")"
  init_n="$(body_lines init_config.txt)"; final_n="$(body_lines final_config.txt)"
  (( init_n >= 1 && init_n <= RANDOM_N )) || fail "init_config has $init_n objects (input had $RANDOM_N)"
  (( init_n < RANDOM_N )) || fail "expected some initial collisions in this dense scenario, but none were resolved"
  (( final_n <= init_n )) || fail "final_config has more objects ($final_n) than init_config ($init_n)"
  for f in init_config.txt final_config.txt; do
    # prints: total mass, #lines with non-positive mass/radius, #lines outside the enclosure
    stats="$(awk -v w="$RANDOM_W" -v h="$RANDOM_H" -v e="$TOL" '
      NR > 1 { sub(/^[0-9]+:[ ]*/, "")
               s += $1; if ($1 <= 0 || $2 <= 0) np++
               if ($3 < $2 - e || $3 > w - $2 + e || $4 < $2 - e || $4 > h - $2 + e) out++ }
      END { printf "%.6f %d %d", s, np, out }' "$CASE_DIR/$f")"
    read -r m np out <<< "$stats"
    close "$m" "$in_mass" || fail "$f: total mass $m differs from input total $in_mass (mass must be conserved)"
    (( np == 0 ))  || fail "$f: $np objects with non-positive mass or radius"
    (( out == 0 )) || fail "$f: $out objects outside the enclosure"
  done
}

t_random_deterministic() {
  local d1 d2
  new_case; d1="$CASE_DIR"
  gen_random 120 7 "$RANDOM_W" "$RANDOM_H" > "$CASE_DIR/input/input.txt"
  run_prog 10 "$RANDOM_W" "$RANDOM_H" 0.25; assert_rc 0
  new_case; d2="$CASE_DIR"
  cp "$d1/input/input.txt" "$d2/input/input.txt"
  run_prog 10 "$RANDOM_W" "$RANDOM_H" 0.25; assert_rc 0
  [[ -f "$d1/final_config.txt" && -f "$d2/final_config.txt" ]] || { fail "final_config.txt missing"; return; }
  cmp -s "$d1/init_config.txt"  "$d2/init_config.txt"  || fail "two runs produced different init_config.txt"
  cmp -s "$d1/final_config.txt" "$d2/final_config.txt" || fail "two runs produced different final_config.txt"
}

#  AOS and SOA must compute the same thing 
x_aos_soa_equivalence() {
  local da db msg f
  EXE="${EXE_OF[aos]}"; PROG=aos; new_case; da="$CASE_DIR"
  gen_random 150 11 "$RANDOM_W" "$RANDOM_H" > "$CASE_DIR/input/input.txt"
  run_prog 5 "$RANDOM_W" "$RANDOM_H" 0.25
  [[ "$RC" -eq 0 ]] || { fail "grav-aos exit code $RC$(stderr_hint)"; return; }
  EXE="${EXE_OF[soa]}"; PROG=soa; new_case; db="$CASE_DIR"
  cp "$da/input/input.txt" "$db/input/input.txt"
  run_prog 5 "$RANDOM_W" "$RANDOM_H" 0.25
  [[ "$RC" -eq 0 ]] || { fail "grav-soa exit code $RC$(stderr_hint)"; return; }
  for f in init_config.txt final_config.txt; do
    if [[ ! -f "$da/$f" || ! -f "$db/$f" ]]; then fail "$f missing in one of the programs"; continue; fi
    msg="$(configs_close "$da/$f" "$db/$f")" || fail "$f differs between grav-aos and grav-soa: $msg"
  done
}