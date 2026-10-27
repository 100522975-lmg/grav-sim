#include <cstdlib>
#include <exception>
#include <iostream>
#include <span>
#include <string>
#include <vector>

#include <progargs.hpp>

// user includes
#include "soa.hpp"
#include "simulation.hpp"
#include "common.hpp"
#include "writer.hpp"
#include "rebound.hpp"
#include "collisions.hpp"

namespace {
  [[nodiscard]] auto generate_write_args(const common::program_arguments& progargs, ParticleSOA& particles) {
    return writer::write_args {
        progargs.width(),
        progargs.height(),
        progargs.time_step(),
        std::span<double>{particles.mass},
        std::span<double>{particles.radius},
        std::span<common::point>{particles.position},
        std::span<common::point>{particles.velocity},
        progargs.num_iterations(),
    };
  }
}

int main(int argc, char ** argv) try {
  std::span const spnargs{argv, static_cast<std::size_t>(argc)};
  std::vector<std::string> const vecargs{spnargs.begin(), spnargs.end()};
  common::program_arguments const progargs{vecargs};
  // initialize particles
  const std::string path = "input/input.txt";
  const auto particle_data = read_particles(path, progargs.width(), progargs.height());
  ParticleSOA particles{particle_data};
  // Write initial configuration to file
  const std::string init_file = "init_config.txt"; 
  writer::write_config(generate_write_args(progargs, particles), init_file);
  // force + acceleration init
  std::vector<common::point> force;
  std::vector<common::point> acceleration; 
  physics::resolve_collisions(particles);
  for (std::size_t i = 0; i < progargs.num_iterations(); ++i) { // main loop
    // compute force and acceleration vectors
    force = physics::accumulate_force_pairs(particles);
    acceleration = physics::compute_acceleration(particles, force);
    // Update velocity and position
    update::all(particles, acceleration, progargs.time_step()); 
    // apply rebound
    const auto rebound_args = std::views::zip(particles.position, particles.velocity, particles.radius);
    for (auto [p,v,r] : rebound_args) {
    common::rebound_effect(p, v, r, common::point{progargs.width(), progargs.height()});
    }
    // find collisions
    physics::resolve_collisions(particles);
  } 
  // write result to final_config.txt
  const std::string final_file = "final_config.txt";
  writer::write_config(generate_write_args(progargs, particles),
                       final_file);
} catch (std::exception const & e) {
  std::cerr << "grav-soa: " << e.what() << '\n';
  return EXIT_FAILURE;
} catch (...) {
  std::cerr << "grav-soa: Unknown exception" << '\n';
  return EXIT_FAILURE; }
