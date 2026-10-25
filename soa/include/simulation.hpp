#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "soa.hpp"

namespace physics {
    [[nodiscard]] std::vector<common::point> accumulate_force_pairs(const ParticleSOA & particles) ;

    [[nodiscard]] std::vector<common::point> compute_acceleration(const ParticleSOA & particles, const std::vector<common::point>& force) ;
}

namespace update {
    void all(ParticleSOA & particles, const std::vector<common::point>& acceleration, double dt);
}

#endif
