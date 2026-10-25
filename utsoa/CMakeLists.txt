set(COMMON_SRC_FILES 
  "${CMAKE_SOURCE_DIR}/soa/src/soa.cpp" 
  "${CMAKE_SOURCE_DIR}/soa/src/simulation.cpp" 
)

set(CURRENT_DIR_SRC_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/test-particle.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_collisions.cpp"
)

add_unit_test_target(
  TARGET_NAME utsoa
  SOURCE_FILES ${COMMON_SRC_FILES} ${CURRENT_DIR_SRC_FILES}
  LIBRARY_FILTER soa
  COVERAGE_DIR coverage-soa
  LIBRARY_TO_LINK common
  INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/soa/include"
)
