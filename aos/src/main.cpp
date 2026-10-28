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

namespace vw = std::views;

namespace{
  [[nodiscard]] auto transform_particles(const std::vector<particle>& particles, const common::program_arguments& progargs) {
    
    return writer::write_args {
      particles | vw::transform(&particle::id),
      particles | vw::transform(&particle::mass),
      particles | vw::transform(&particle::radius),
      particles | vw::transform(&particle::position),
      particles | vw::transform(&particle::velocity),
      progargs.width(),
      progargs.height(),
      progargs.time_step(),
      particles.size(),
    };
  }
} //anonymous namespace

int main(int argc, char ** argv) try {
  // Parse CLI args
  std::span const spnargs{argv, static_cast<std::size_t>(argc)};
  std::vector<std::string> const vecargs{spnargs.begin(), spnargs.end()};
  common::program_arguments const progargs{vecargs};            
  
  //read valid particles fo input, the id of each one is its position
  auto const rows=read_particles("input/input.txt",progargs.width(),progargs.height());
  std::vector<particle> particles;
  particles.reserve(rows.size());
  for(std::size_t id=0;id<rows.size();++id){
    particles.push_back(make_particle(rows.at(id),id));
  }
  //save current configuration
  auto const save=[&](std::string const& path){
    writer::write_config(transform_particles(particles,progargs),path);
  };
  //resolve collisions
  aos::resolve_collisions(particles);
  save("init_config_aos.txt");
  //run simulation
  for(std::size_t i=0;i<progargs.num_iterations();++i){
    step(particles,progargs.width(),progargs.height(),progargs.time_step());
    aos::resolve_collisions(particles);
  }
  save("final_config_aos.txt");
  // Error handling
} catch (std::exception const & e) {
  std::cerr << "grav-aos: " << e.what() << '\n';
  return EXIT_FAILURE;

} catch (...) {
  std::cerr << "grav-aos: Unknown exception" << '\n';
  return EXIT_FAILURE;
}
