#include "common.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <regex>


// Reads path and uses a regex to parse valid particle info 
std::vector<double> parse_particle_line(const std::string& line) {
  // regex definitions: force checks positive magnitudes
  const std::string num = R"((\d+\.\d+))";
  //since velocities can be negative we check for those values in velocities
  const std::string vels = R"((-?\d+\.\d+))";
  static const std::regex pattern(num + " " + num + " " + num + " " +
                      num + " " + vels + " " + vels + R"(\s*)"); 
  std::smatch match;
 
  // regex match line and validate input
  if (!std::regex_match(line, match, pattern)) {
    return {}; // empty vector i.e. skip due to bad match
  }

  std::vector<double> parsed;
  //preallocate memory
  parsed.reserve(6);

  for (std::size_t i = 0; i < 6; i++) {
    parsed.push_back(std::stod(match.str(i+1)));
  }

  return parsed;
}

//checks particle inside enclosure using the given formulas
bool check_bound(const std::vector<double>& particle, double width, double height){
  const double radius = particle.at(1);
  const double x = particle.at(2);
  const double y = particle.at(3);

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

std::vector<std::vector<double>> read_particles(const std::string& input_path, double width, double height) {  
  std::vector<std::vector<double>> particles;
  std::ifstream file(input_path);
  std::string line;
  
  while (std::getline(file, line)) {
    const auto parsed = parse_particle_line(line);
    //  check if the particle is inside the enclosure and  radius and mass are strictly positive
    if (parsed.at(0) > 0.0 and parsed.at(1) > 0.0 and check_bound(parsed, width, height)) {
      particles.push_back(parsed);
    } else {
      particles.push_back({}); // we need bad matches to correctly assign ids
    }
  }
  return particles;
}
