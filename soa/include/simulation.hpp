#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "soa.hpp"
#include <span>

namespace physics {
    /* Accumulate force pairs
     * Take vector of points: forces 
     * Directly accumulate all forces into f_i reference
     * We use Newton's third law to skip the first i+1 elements
     * Write to the vector of forces
     */
    void accumulate_force_pairs(ParticleSOA const & particles, std::vector<common::point> & force);
    
    /* Compute acceleration
     * Take a vector of points: acceleration
     * Use the previous function to obtain forces
     * Divide each component of point by each particle's mass
     * Write to the vector of accelerations 
     */
    void compute_acceleration(ParticleSOA const & particles, std::span<const common::point> forces, std::vector<common::point> & acceleration); 
}

namespace update {
    /* Update all
     * This function uses two helpers defined in an anon. namespace
     * Error checks vector sizes
     * Zips pos. vel. acc. and iterates over the zipped range
     * Updates velocity first
     * Updates position second
     */
    void all(ParticleSOA & particles, std::span<const common::point> acceleration, double dt);
}

#endif
