cmake_minimum_required(VERSION 4.0)
project(grav LANGUAGES CXX)

# C++ version configuration
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# Apply platform-specific warning flags to the interface target
add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wextra>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wpedantic>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-pedantic-errors>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wconversion>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wsign-conversion>
)

# USER ADDED
add_library(project_optimizations INTERFACE)
target_compile_options(project_optimizations INTERFACE
    $<$<CONFIG:Release>:$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3 -ffast-math>>
    $<$<CONFIG:Release>:$<$<CXX_COMPILER_ID:MSVC>:/O2 /fp:fast>>
)


# Fetch Microsoft GSL and GoogleTest
include(FetchContent)

FetchContent_Declare(
  GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        v5.0.0  # Use the latest stable version
  GIT_SHALLOW    TRUE
)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.18.0  # Use the latest stable version
  GIT_SHALLOW    TRUE
)

# Configure GoogleTest options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # For Windows compatibility
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)         # Don't install GoogleTest
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)           # Disable Google Mock

FetchContent_MakeAvailable(GSL googletest)

if (ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy-22 clang-tidy)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--header-filter=.*")
    else()
        message(WARNING "clang-tidy not found")
    endif()
endif()

# Enable testing
enable_testing()
include(GoogleTest)

# Include test utils
include(cmake/TestUtils.cmake)

add_subdirectory(common)
add_subdirectory(aos)
add_subdirectory(soa)
add_subdirectory(utcommon)
add_subdirectory(utaos)
add_subdirectory(utsoa)
