#include <cstdlib>
#include <exception>
#include <iostream>
#include <print>
#include <span>
#include <string>
#include <vector>

#include <progargs.hpp>

// user includes
#include "soa.hpp"
#include "common.hpp"

int main(int argc, char ** argv) try {
  std::span const spnargs{argv, static_cast<std::size_t>(argc)};
  std::vector<std::string> const vecargs{spnargs.begin(), spnargs.end()};
  common::program_arguments const progargs{vecargs};

  std::println("grav-soa:");
  std::println("\tnum_iterations: {}\n\twidth: {}\n\theight: {}\n\ttime_step: {}",
               progargs.num_iterations(), progargs.width(), progargs.height(),
               progargs.time_step());

  // CODE START  
  const std::string path = "inputs/input.txt";
  const auto particle_data = read_particles(&path);

   


  // CODE END
} catch (std::exception const & e) {
  std::cerr << "grav-soa: " << e.what() << '\n';
  return EXIT_FAILURE;

} catch (...) {
  std::cerr << "grav-soa: Unknown exception" << '\n';
  return EXIT_FAILURE;
}
