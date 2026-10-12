#ifndef COMMON_HPP
#define COMMON_HPP

#include <string> 
#include <vector>


std::vector<double> parse_particle_line(const std::string& line);

bool check_bound(const std::vector<double>& particle, double width, double height);

std::vector<std::vector<double>> read_particles(const std::string& input_path);

#endif
