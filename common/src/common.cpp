 #include "common.hpp"

#include <regex>
#include <iostream>

// Reads path and uses a regex to parse valid particle info 
std::vector<double> parse_particle_line(const std::string& line) {
  // regex definitions: force checks positive magnitudes
  const std::string num = R"((\d+\.\d+))";
  //since velocities can be negative we check for those values in velocities
  const std::string vels = R"((-?\d+\.\d+))";
  const std::regex pattern(num + " " + num + " " + num + " " +
                      num + " " + vels + " " + vels);
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

//checks particle inside enclosure using the given formulas
bool check_bound(const std::vector<double>& particle, double width, double height){
  const double radius = particle[1];
  const double x = particle[2];
  const double y = particle[3];

  //check position of particle both in the x and y axis
  if (x<radius or x>width-radius){
    return false;
  }
  if (y<radius or y>height-radius){
    return false;
  }
  //passed both checks so its inside
  return true;
}

std::vector<std::vector<double>> read_particles(const std::string& input_path) {  
  return {};
}
