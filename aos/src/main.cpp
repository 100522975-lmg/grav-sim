#include <cstdlib>
#include <exception>
#include <iostream>
#include <span>
#include <vector>

#include <progargs.hpp>

// User includes
#include "collisions.hpp"
#include "common.hpp"
#include "particle.hpp"
#include "simulation.hpp"
#include "writer.hpp"

namespace {
  using namespace std::views;
  [[nodiscard]] auto transform_particles(const std::vector<particle>& particles, const common::program_arguments& progargs) {
    
    return writer::write_args {
      particles | transform(&particle::id),
      particles | transform(&particle::mass),
      particles | transform(&particle::radius),
      particles | transform(&particle::position),
      particles | transform(&particle::velocity),
      progargs.width(),
      progargs.height(),
      progargs.time_step(),
      particles.size(),
    };
  }
} //anonymous namespace

/* -- Main --
 * Unpack progargs values for better cache optimization
 * Read input file into a buffer
 * Generate particles based on the input file's rows
 * Setup write-to-file lambda function
 * Resolve initial collisions
 * Write initial configuration
 * Loop: step, resolve collisions, remove unused particles
 * Write final configuration
 * Error handling*/
int main(int argc, char ** argv) try {
  std::span const spnargs{argv, static_cast<std::size_t>(argc)};
  std::vector<std::string> const vecargs{spnargs.begin(), spnargs.end()};
  common::program_arguments const progargs{vecargs};            

  auto const rows=read_particles("input/input.txt", progargs.width(), progargs.height());
  std::vector<particle> particles;
  particles.reserve(rows.size());
  for(std::size_t id = 0; auto const& row : rows) {
    if (row.empty()) { id++; continue; }
    particles.push_back(make_particle(row, id++));
  }
  auto const save=[&](std::string const& path){
    writer::write_config(transform_particles(particles, progargs),path);
  };
  aos::resolve_collisions(particles);
  remove_unused(particles);
  save("init_config_aos.txt");
  
  std::size_t const N = particles.size();
  std::vector<common::point> forces, accelerations;
  forces.reserve(N); accelerations.reserve(N);

  for(std::size_t i = 0; i < progargs.num_iterations(); ++i) {
    step(particles, forces, accelerations, progargs);
    aos::resolve_collisions(particles);
    remove_unused(particles);
  }
  save("final_config_aos.txt");
} catch (std::exception const & e) {
  std::cerr << "grav-aos: " << e.what() << '\n';
  return EXIT_FAILURE;

} catch (...) {
  std::cerr << "grav-aos: Unknown exception" << '\n';
  return EXIT_FAILURE;
}
