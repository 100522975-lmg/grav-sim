add_executable(grav-aos)
target_sources(grav-aos PRIVATE 
    src/main.cpp
    include/simulation.hpp
    include/particle.hpp
#    include/file1.hpp
#    src/file1.cpp
#    include/file2.hpp
#    src/file2.cpp
)

target_include_directories(grav-aos PRIVATE 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(grav-aos PRIVATE project_warnings GSL common)