set(COMMON_SRC_FILES 
  "${CMAKE_SOURCE_DIR}/common/src/progargs.cpp"
)

set(CURRENT_DIR_SRC_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/test_progargs.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_point.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_gravity.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_rebound.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_common.cpp"
)

add_unit_test_target(
  TARGET_NAME utcommon
  SOURCE_FILES ${COMMON_SRC_FILES} ${CURRENT_DIR_SRC_FILES}
  LIBRARY_FILTER common
  COVERAGE_DIR coverage-common
  LIBRARY_TO_LINK common
)