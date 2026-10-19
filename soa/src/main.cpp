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
#include "rebound.hpp"

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
  const std::string init_file = "init_config.txt"; // Write initial configuration to file
  writer::write_config(
    writer::write_args {
      progargs.width(),
      progargs.height(),
      progargs.time_step(),
      std::span<double>{particles.mass},
      std::span<double>{particles.radius},
      std::span<common::point>{particles.position},
      std::span<common::point>{particles.velocity},
      progargs.num_iterations()
    },
    init_file
  ); // force + acceleration init
  std::vector<common::point> force;
  std::vector<common::point> acceleration; 
  for (std::size_t i = 0; i < progargs.num_iterations(); ++i) { // main loop
    force = particles.accumulate_force_pairs();
    acceleration = particles.compute_acceleration(force);
    particles.update_all(acceleration, progargs.time_step());// update velocity + position 
    common::rebound_effect(particles.position.at(i), particles.velocity.at(i), particles.radius.at(i), common::point{progargs.width(), progargs.height()});// apply rebound
  } // write result to final_config.txt
  const std::string final_path = "final_config.txt";
} catch (std::exception const & e) {
  std::cerr << "grav-soa: " << e.what() << '\n';
  return EXIT_FAILURE;
} catch (...) {
  std::cerr << "grav-soa: Unknown exception" << '\n';
  return EXIT_FAILURE; }
