add_executable(grav-soa)
target_sources(grav-soa PRIVATE 
    src/main.cpp
    src/soa.cpp
    include/soa.hpp
)

target_include_directories(grav-soa PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(grav-soa PRIVATE project_warnings GSL common)
