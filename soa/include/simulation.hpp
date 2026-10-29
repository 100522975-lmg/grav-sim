#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "soa.hpp"

namespace physics {
    /* Accumulate force pairs
     * Initialize a vector of points: forces
     * Directly accumulate all forces into f_i reference
     * We use Newton's third law to skip the first i+1 elements
     * Return the vector of accumulated forces
     */
    [[nodiscard]] std::vector<common::point> accumulate_force_pairs(const ParticleSOA & particles) ;
    
    /* Compute acceleration
     * Initialize a vector of points: acceleration
     * Use the previous function to obtain forces
     * Divide each component of point by each particle's mass
     * Return the vector of accelerations
     */
    [[nodiscard]] std::vector<common::point> compute_acceleration(const ParticleSOA & particles, const std::vector<common::point>& force) ;
}

namespace update {
    /* Update all
     * This function uses two helpers defined in an anon. namespace
     * Error checks vector sizes
     * Zips pos. vel. acc. and iterates over the zipped range
     * Updates velocity first
     * Updates position second
     */
    void all(ParticleSOA & particles, const std::vector<common::point>& acceleration, double dt);
}

#endif
