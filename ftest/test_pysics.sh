# ftest/test_pysics.sh - output format, motion, rebound and gravity

t_output_format() {
  new_case
  write_input <<'EOF'
10.0 1.0 50.0 50.0 2.5 -1.5
20.0 2.0 80.0 80.0 0.0 0.0
EOF
  run_prog 3 1000 500 0.25
  assert_rc 0
  local f
  for f in init_config.txt final_config.txt; do
    if ! assert_file "$f"; then continue; fi
    assert_format "$f"
    assert_header "$f" "1000.000 500.000 0.250 2"
  done
}

t_stationary_particle() {
  new_case
  write_input <<< "10.0 1.0 50.0 50.0 0.0 0.0"
  run_prog 5 100 100 0.25
  assert_rc 0
  assert_file init_config.txt  && { assert_count init_config.txt 1;  assert_particle init_config.txt 0 10 1 50 50 0 0; }
  assert_file final_config.txt && { assert_count final_config.txt 1; assert_particle final_config.txt 0 10 1 50 50 0 0; }
}

t_uniform_motion() {
  # init_config must show the state BEFORE the simulation, final_config AFTER.
  new_case
  write_input <<< "10.0 1.0 50.0 50.0 2.0 -1.0"
  run_prog 4 100 100 0.5
  assert_rc 0
  assert_file init_config.txt  && { assert_count init_config.txt 1;  assert_particle init_config.txt 0 10 1 50 50 2 -1; }
  assert_file final_config.txt && { assert_count final_config.txt 1; assert_particle final_config.txt 0 10 1 54 48 2 -1; }
}

#  rebound on the enclosure walls
t_rebound_all_walls() {
  new_case
  write_input <<'EOF'
1.0 1.0 1.5 50.0 -2.0 0.0
1.0 1.0 98.5 20.0 2.0 0.0
1.0 1.0 30.0 1.5 0.0 -3.0
1.0 1.0 70.0 98.5 0.0 3.0
1.0 1.0 98.5 98.5 3.0 3.0
EOF
  run_prog 1 100 100 1.0
  assert_rc 0
  if ! assert_file final_config.txt; then return; fi
  assert_count final_config.txt 5
  assert_particle final_config.txt 0 1 1  1  50  2  0   # left wall
  assert_particle final_config.txt 1 1 1 99  20 -2  0   # right wall
  assert_particle final_config.txt 2 1 1 30   1  0  3   # top (y = 0)
  assert_particle final_config.txt 3 1 1 70  99  0 -3   # bottom (y = height)
  assert_particle final_config.txt 4 1 1 99  99 -3 -3   # corner: both axes
}

t_no_rebound_when_inside() {
  # close to the wall (x - r = 0.5 > 0) and moving away from it: nothing happens
  new_case
  write_input <<< "1.0 1.0 1.5 50.0 2.0 0.0"
  run_prog 1 100 100 1.0
  assert_rc 0
  if ! assert_file final_config.txt; then return; fi
  assert_particle final_config.txt 0 1 1 3.5 50 2 0
}

#  gravitational attraction (G = 6.6743e-11)
#   With m_other = 1e16 and d = 100:  a = G * 1e16 / 100^2 = 66.743
t_gravity_unequal_masses() {
  new_case
  write_input <<'EOF'
10000000000000000.0 1.0 500.0 500.0 0.0 0.0
1.0 1.0 600.0 500.0 0.0 0.0
EOF
  run_prog 1 1000 1000 1.0
  assert_rc 0
  if ! assert_file final_config.txt; then return; fi
  assert_count final_config.txt 2
  assert_particle final_config.txt 0 1e16 1 500     500 0       0   # heavy one barely moves
  assert_particle final_config.txt 1 1    1 533.257 500 -66.743 0   # light one falls towards it
}

t_gravity_equal_masses() {
  # Newton's third law: equal and opposite accelerations
  new_case
  write_input <<'EOF'
10000000000000000.0 1.0 450.0 500.0 0.0 0.0
10000000000000000.0 1.0 550.0 500.0 0.0 0.0
EOF
  run_prog 1 1000 1000 1.0
  assert_rc 0
  if ! assert_file final_config.txt; then return; fi
  assert_count final_config.txt 2
  assert_particle final_config.txt 0 1e16 1 516.743 500  66.743 0
  assert_particle final_config.txt 1 1e16 1 483.257 500 -66.743 0
}

t_gravity_direction_2d() {
  # (dx, dy) = (60, 80) -> d = 100, unit vector (0.6, 0.8): a = (-40.046, -53.394)
  new_case
  write_input <<'EOF'
10000000000000000.0 1.0 400.0 400.0 0.0 0.0
1.0 1.0 460.0 480.0 0.0 0.0
EOF
  run_prog 1 1000 1000 1.0
  assert_rc 0
  if ! assert_file final_config.txt; then return; fi
  assert_count final_config.txt 2
  assert_particle final_config.txt 1 1 1 419.954 426.606 -40.046 -53.394
}