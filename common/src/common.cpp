 #include "common.hpp"

#include <regex>
#include <iostream>

// Reads path and uses a regex to parse valid particle info 
std::vector<double> parse_particle_line(const std::string& line) {
  // regex definitions: force checks positive magnitudes
  const std::string num = R"((\d+\.\d+))";
  const std::regex pattern(num + " " + num + " " + num + " " +
                      num + " " + num + " " + num);
  std::smatch match;
 
  // regex match line and validate input
  if (!std::regex_match(line, match, pattern)) {
    return {}; // empty vector i.e. skip due to bad match
  }

  std::vector<double> parsed;

  for (int i = 0; i < 6; i++) {
    parsed.push_back(std::stod(match[i+1].str()));
  }

  return parsed;
}

std::vector<std::vector<double>> read_particles(const std::string& input_path) {
  return {};
}
