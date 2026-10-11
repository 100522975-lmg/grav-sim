#ifndef COMMON_HPP
#define COMMON_HPP

#include <string> 
#include <vector>

std::vector<double> parse_particle_line(const std::string& path);

std::vector<std::vector<double>> read_particles(std::string input_path);

#endif
