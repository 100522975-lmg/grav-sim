#ifndef COMMON_HPP
#define COMMON_HPP

#include <string> 
#include <vector>
#include <limits>

namespace constants {
    inline constexpr double G = 6.67430e-11;
    inline constexpr std::size_t UNUSED_ID = std::numeric_limits<std::size_t>::max();
}

std::vector<double> parse_particle_line(const std::string& line);

bool check_bound(const std::vector<double>& particle, double width, double height);

std::vector<std::vector<double>> read_particles(const std::string& input_path, double width, double height);

#endif
