add_library(common STATIC)
target_sources(common PRIVATE
    src/progargs.cpp
    src/common.cpp
    include/point.hpp
    include/common.hpp
)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)