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
#include "writer.hpp"
#include <ranges>


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
  ParticleSOA particles{particle_data};
  // Write initial configuration to file
  const std::string init_file = "init_config.txt";
  common::writer init{init_file};
  init.format_header(progargs.width(), progargs.height(), progargs.time_step(), particles.id.size());
  for (const auto&& [m, r, p, v] : std::views::zip(particles.mass, particles.radius, particles.position, particles.velocity)) {
    init.format_particle_line(m, r, p, v);
  }
  // force + acceleration init
  std::vector<common::point> force;
  std::vector<common::point> acceleration; 
  // main loop
  for (std::size_t i = 0; i < progargs.num_iterations(); ++i) {
    force = particles.accumulate_force_pairs();
    acceleration = particles.compute_acceleration(force);
    // update velocity + position 
    particles.update_all(acceleration, progargs.time_step());
    // apply rebound
    particles.apply_rebound(progargs.width(), progargs.height());
  }
} catch (std::exception const & e) {
  std::cerr << "grav-soa: " << e.what() << '\n';
  return EXIT_FAILURE;
} catch (...) {
  std::cerr << "grav-soa: Unknown exception" << '\n';
  return EXIT_FAILURE;
}
