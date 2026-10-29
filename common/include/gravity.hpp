#ifndef COMMON_GRAVITY_HPP
#define COMMON_GRAVITY_HPP

#include <cmath>
#include "point.hpp"
#include "common.hpp"

namespace common {
    /* Pair force
     * Small function to compute the force between two particles
     * Using Newton's formula:
     * F_{ij} = G * \frac{m_{i} * m_{j}}{||p_{j} - p_{i}||^2} * \frac{p_{j} - p_{i}}{||p_{j} - p_{i}||}
     * Avoids using std::pow in a hot loop
     * Program avoids computing F_{i,i} so we save a branch checking
     * division by 0 
     */
    [[nodiscard]] inline point pair_force(double mass_i, double mass_j, point const & p_i, point const & p_j){ 
        double const pj_pi_x = p_j.x - p_i.x; 
        double const pj_pi_y = p_j.y - p_i.y; 

        double const dist_sq = pj_pi_x * pj_pi_x + pj_pi_y * pj_pi_y;

        double const norm_pi_pj = std::sqrt(dist_sq);
        double const norm_pi_pj_mult= dist_sq*norm_pi_pj;
        double const inv_norm_pi_pj=1.0/norm_pi_pj_mult;
        double const factor = constants::G * mass_i * mass_j * inv_norm_pi_pj;

        return {factor * pj_pi_x, factor * pj_pi_y}; 
    }
}

#endif
