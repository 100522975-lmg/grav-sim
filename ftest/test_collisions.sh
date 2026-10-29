# ftest/test_collisions.sh - collisions

# collide2 <label> <line-0> <line-1> <expected init line (6 numbers)> <expected final line>
collide2() {
  local label=$1 l0=$2 l1=$3 ei=$4 ef=$5
  new_case
  printf '%s\n%s\n' "$l0" "$l1" > "$CASE_DIR/input/input.txt"
  run_prog 1 100 100 1.0
  [[ "$RC" -eq 0 ]] || { fail "$label: exit code $RC$(stderr_hint)"; return; }
  # shellcheck disable=SC2086
  if assert_file init_config.txt; then
    assert_count init_config.txt 1; assert_particle init_config.txt 0 $ei
  fi
  # shellcheck disable=SC2086
  if assert_file final_config.txt; then
    assert_count final_config.txt 1; assert_particle final_config.txt 0 $ef
  fi
}

t_collision_heavier_absorbs() {
  # the SECOND particle is heavier, so it absorbs the first
  collide2 "mass" "5.0 1.0 50.0 50.0 1.0 0.0" "10.0 2.0 51.0 50.0 0.0 2.0" \
           "15 2 51 50 1 2" "15 2 52 52 1 2"
}
t_collision_equal_mass_larger_radius() {
  collide2 "radius" "5.0 1.0 50.0 50.0 1.0 0.0" "5.0 2.0 51.0 50.0 0.0 2.0" \
           "10 2 51 50 1 2" "10 2 52 52 1 2"
}
t_collision_equal_mass_radius_lower_id() {
  collide2 "id" "5.0 1.0 50.0 50.0 1.0 0.0" "5.0 1.0 51.0 50.0 0.0 2.0" \
           "10 1 50 50 1 2" "10 1 51 52 1 2"
}

t_collision_distance_threshold() {
  # collide only if distance < r_i + r_j (here the sum of radii is 3)
  local label d n
  for d in 3.0:2 2.9:1 3.1:2; do
    n="${d#*:}"; d="${d%:*}"; label="distance $d"
    new_case
    printf '5.0 1.0 50.0 50.0 0.0 0.0\n5.0 2.0 %s 50.0 0.0 0.0\n' "$(awk -v d="$d" 'BEGIN { printf "%.1f", 50 + d }')" \
      > "$CASE_DIR/input/input.txt"
    run_prog 1 100 100 1.0
    [[ "$RC" -eq 0 ]] || { fail "$label: exit code $RC$(stderr_hint)"; continue; }
    if assert_file init_config.txt; then
      local hn; hn="$(head -n1 "$CASE_DIR/init_config.txt" | awk '{ print $4 }')"
      [[ "$hn" == "$n" ]] || fail "$label: init_config has $hn objects, expected $n"
    fi
  done
}

t_collision_cascade_updates_mass() {
  # (0,1): B(m=2) absorbs A(m=1) -> B has m=3.  Re-scan: (B,C) tie on mass and
  # radius -> lower id (B) absorbs C.  Using stale masses would let C win.
  new_case
  write_input <<'EOF'
1.0 1.0 50.0 50.0 1.0 0.0
2.0 1.0 51.5 50.0 0.0 1.0
3.0 1.0 53.0 50.0 0.0 0.5
EOF
  run_prog 1 100 100 1.0
  assert_rc 0
  assert_file init_config.txt  && { assert_count init_config.txt 1;  assert_particle init_config.txt 0 6 1 51.5 50 1 1.5; }
  assert_file final_config.txt && { assert_count final_config.txt 1; assert_particle final_config.txt 0 6 1 52.5 51.5 1 1.5; }
}

t_collision_lexicographic_order() {
  # pairs must be examined as (0,1) first.  P0(3) absorbs P1(1) -> 4, then
  # P0(4) absorbs P2(3).  Checking (1,2) first would leave P2's position.
  new_case
  write_input <<'EOF'
3.0 1.0 50.0 50.0 1.0 0.0
1.0 1.0 50.5 50.0 0.0 1.0
3.0 1.0 51.0 50.0 0.5 0.5
EOF
  run_prog 1 100 100 1.0
  assert_rc 0
  assert_file init_config.txt  && { assert_count init_config.txt 1;  assert_particle init_config.txt 0 7 1 50 50 1.5 1.5; }
  assert_file final_config.txt && { assert_count final_config.txt 1; assert_particle final_config.txt 0 7 1 51.5 51.5 1.5 1.5; }
}

t_collision_during_simulation() {
  # two unit particles approach each other at 1 unit/step each (distance 21 - 2k).
  # k = 9 -> distance 3 (no collision); k = 10 -> distance 1 -> they merge.
  new_case
  write_input <<'EOF'
1.0 1.0 40.0 50.0 1.0 0.0
1.0 1.0 61.0 50.0 -1.0 0.0
EOF
  run_prog 9 100 100 1.0
  assert_rc 0
  if assert_file final_config.txt; then
    assert_count final_config.txt 2
    assert_particle final_config.txt 0 1 1 49 50  1 0
    assert_particle final_config.txt 1 1 1 52 50 -1 0
  fi
  new_case
  write_input <<'EOF'
1.0 1.0 40.0 50.0 1.0 0.0
1.0 1.0 61.0 50.0 -1.0 0.0
EOF
  run_prog 12 100 100 1.0
  assert_rc 0
  if assert_file init_config.txt; then assert_count init_config.txt 2; fi
  if assert_file final_config.txt; then
    # merged at step 10 (v = 0) and then stays at the absorber's position
    assert_count final_config.txt 1
    assert_particle final_config.txt 0 2 1 50 50 0 0
  fi
}