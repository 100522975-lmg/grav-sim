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

  const std::string path = "inputs/input.txt";
  const auto particle_data = read_particles(path, progargs.width(), progargs.height());
  
  // initialize particles
  ParticleSOA particles(particle_data);
  
  // TODO: Write initial configuration to file
  
  // force + acceleration init
  std::vector<common::point> force;
  std::vector<common::point> acceleration;
  
  // main loop
  for (auto i : progargs.num_iterations()) {
    force = ParticleSOA::accumulate_force_pairs();
    acceleration = ParticleSOA::compute_acceleration(force);

    // update velocity
    ParticleSOA::update_velocity(acceleration);

    // update position
    ParticleSOA::update_position();

    // apply rebound
    ParticleSOA::apply_rebound(progargs.width(), progargs.height());
  }

} catch (std::exception const & e) {
  std::cerr << "grav-soa: " << e.what() << '\n';
  return EXIT_FAILURE;

} catch (...) {
  std::cerr << "grav-soa: Unknown exception" << '\n';
  return EXIT_FAILURE;
}
