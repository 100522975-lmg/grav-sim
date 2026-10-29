# ftest/test_input.sh - validation of the initial configuration

t_input_discards_out_of_range() {
  new_case
  write_input <<'EOF'
10.0 1.0 50.0 50.0 0.0 0.0
0.0 1.0 20.0 20.0 0.0 0.0
10.0 0.0 30.0 30.0 0.0 0.0
10.0 5.0 2.0 50.0 0.0 0.0
10.0 5.0 98.0 50.0 0.0 0.0
10.0 5.0 50.0 2.0 0.0 0.0
10.0 5.0 50.0 98.0 0.0 0.0
20.0 2.0 80.0 80.0 0.0 0.0
EOF
  run_prog 1 100 100 1.0
  assert_rc 0
  assert_file init_config.txt || return
  assert_count init_config.txt 2
  assert_particle init_config.txt 0 10 1 50 50 0 0
  assert_particle init_config.txt 1 20 2 80 80 0 0
}

t_input_discards_negative_values() {
  new_case
  write_input <<'EOF'
-5.0 1.0 20.0 20.0 0.0 0.0
10.0 -1.0 30.0 30.0 0.0 0.0
10.0 1.0 -5.0 50.0 0.0 0.0
10.0 1.0 50.0 50.0 0.0 0.0
EOF
  run_prog 1 100 100 1.0
  assert_rc 0
  assert_file init_config.txt || return
  assert_count init_config.txt 1
  assert_particle init_config.txt 0 10 1 50 50 0 0
}

t_input_ignores_malformed_lines() {
  new_case
  write_input <<'EOF'
10.0 1.0 50.0 50.0 0.0 0.0
this is not a particle
10.0 1.0 20.0

10.0 1.0 x 30.0 0.0 0.0
10.0 1.0 40.0 40.0 0.0 0.0 7.0
20.0 2.0 80.0 80.0 0.0 0.0
EOF
  run_prog 1 100 100 1.0
  assert_rc 0
  assert_file init_config.txt || return
  assert_count init_config.txt 2
  assert_particle init_config.txt 0 10 1 50 50 0 0
  assert_particle init_config.txt 1 20 2 80 80 0 0
}

t_input_incomplete_last_line() {
  new_case
  printf '10.0 1.0 50.0 50.0 0.0 0.0\n20.0 2.0 80.0 80.0 0.0 0.0\n30.0 1.0 20.0 20.0 1.0' > "$CASE_DIR/input/input.txt"
  run_prog 1 100 100 1.0
  assert_rc 0
  assert_file init_config.txt || return
  assert_count init_config.txt 2
}

t_input_last_line_without_newline() {
  # a complete last line is valid even if the file does not end with '\n'
  new_case
  printf '10.0 1.0 50.0 50.0 0.0 0.0\n20.0 2.0 80.0 80.0 0.0 0.0' > "$CASE_DIR/input/input.txt"
  run_prog 1 100 100 1.0
  assert_rc 0
  assert_file init_config.txt || return
  assert_count init_config.txt 2
  assert_particle init_config.txt 1 20 2 80 80 0 0
}

t_input_no_valid_objects() {
  # (behaviour inferred from the statement: invalid objects are simply discarded)
  new_case
  write_input <<'EOF'
0.0 1.0 20.0 20.0 0.0 0.0
garbage
10.0 5.0 2.0 50.0 0.0 0.0
EOF
  run_prog 2 100 100 1.0
  assert_rc 0
  local f
  for f in init_config.txt final_config.txt; do
    assert_file "$f" || continue
    assert_header "$f" "100.000 100.000 1.000 0"
    assert_count "$f" 0
  done
}