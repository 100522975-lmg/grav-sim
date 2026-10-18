#include <cstdlib>
#include <exception>
#include <iostream>
#include <print>
#include <span>
#include <vector>

#include <progargs.hpp>

// User includes
#include "writer.hpp"

//#include "common.hpp"
namespace { //anonymous namespace 
  struct Particle {
    double mass, radius;
    common::point position, velocity;
  };
}

/* TODO: HAY QUE DESCOMENTAR ESTO CUANDO SE TERMINE EL MAIN WIRING

using Ref = std::ranges::ref_view<const std::vector<Particle>>;
using MassView = std::ranges::transform_view<Ref, double Particle::*>;
using PointView = std::ranges::transform_view<Ref, common::point Particle::*>;

[[nodiscard]] writer::write_args<MassView, PointView> transform_particles(const std::vector<Particle>& particles, const common::program_arguments& progargs) {
  namespace vw = std::views;
  return writer::write_args {
    progargs.width(),
    progargs.height(),
    progargs.time_step(),
    particles | vw::transform(&Particle::mass),
    particles | vw::transform(&Particle::radius),
    particles | vw::transform(&Particle::position),
    particles | vw::transform(&Particle::velocity),
    progargs.num_iterations()
  };
}
*/

int main(int argc, char ** argv) try {
  // Parse CLI args
  std::span const spnargs{argv, static_cast<std::size_t>(argc)};
  std::vector<std::string> const vecargs{spnargs.begin(), spnargs.end()};
  common::program_arguments const progargs{vecargs};

  std::println("grav-aos:");
  std::println("\tnum_iterations: {}\n\twidth: {}\n\theight: {}\n\ttime_step: {}",
               progargs.num_iterations(), progargs.width(), progargs.height(),
               progargs.time_step());
  
  // Error handling
} catch (std::exception const & e) {
  std::cerr << "grav-aos: " << e.what() << '\n';
  return EXIT_FAILURE;

} catch (...) {
  std::cerr << "grav-aos: Unknown exception" << '\n';
  return EXIT_FAILURE;
}
