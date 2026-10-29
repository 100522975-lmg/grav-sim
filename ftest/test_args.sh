# ftest/test_args.sh - invalid command line

# arg_error <label> <stderr-regex> <args...>
arg_error() {
  local label=$1 re=$2 f first=""
  shift 2
  new_case
  write_input <<< "10.0 1.0 50.0 50.0 0.0 0.0"
  run_prog "$@"
  if [[ "$RC" -ne 1 ]]; then
    fail "$label: exit code is $RC, expected 1 (EXIT_FAILURE)"
  fi
  if [[ ! -s "$CASE_DIR/stderr.txt" ]]; then
    fail "$label: nothing written to standard error"
  else
    IFS= read -r first < "$CASE_DIR/stderr.txt"
    if [[ "$first" != "grav-$PROG:"* ]]; then
      fail "$label: first stderr line should start with 'grav-$PROG:' (got '$first')"
    fi
    if ! grep -Eq "$re" "$CASE_DIR/stderr.txt"; then
      fail "$label: stderr does not match /$re/$(stderr_hint)"
    fi
  fi
  for f in init_config.txt final_config.txt; do
    if [[ -e "$CASE_DIR/$f" ]]; then
      fail "$label: $f was written even though the arguments are invalid"
    fi
  done
}

t_err_arg_count() {
  arg_error "no arguments"  "Expected 4 arguments"
  arg_error "1 argument"    "Expected 4 arguments"  10
  arg_error "3 arguments"   "Expected 4 arguments"  100 1000 500
  arg_error "5 arguments"   "Expected 4 arguments"  100 1000 500 0.25 7
}
t_err_num_iterations() {
  local re="Invalid value for '?num_iterations"
  arg_error "iterations = 0"      "$re"  0    1000 500 0.25
  arg_error "iterations = -5"     "$re"  -5   1000 500 0.25
  arg_error "iterations = abc"    "$re"  abc  1000 500 0.25
  arg_error "iterations = 1.5"    "$re"  1.5  1000 500 0.25
  arg_error "iterations = 10x"    "$re"  10x  1000 500 0.25
}
t_err_width() {
  local re="Invalid value for '?width"
  arg_error "width = -1000" "$re"  100 -1000 2000 0.25
  arg_error "width = 0"     "$re"  100 0     2000 0.25
  arg_error "width = abc"   "$re"  100 abc   2000 0.25
  arg_error "width = 5m"    "$re"  100 5m    2000 0.25
}
t_err_height() {
  local re="Invalid value for '?height"
  arg_error "height = -1"   "$re"  100 1000 -1  0.25
  arg_error "height = 0"    "$re"  100 1000 0   0.25
  arg_error "height = xyz"  "$re"  100 1000 xyz 0.25
}
t_err_time_step() {
  local re="Invalid value for '?time_step"
  arg_error "time_step = -0.25" "$re"  100 1000 500 -0.25
  arg_error "time_step = 0"     "$re"  100 1000 500 0
  arg_error "time_step = abc"   "$re"  100 1000 500 abc
}