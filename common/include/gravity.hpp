#ifndef COMMON_GRAVITY_HPP
#define COMMON_GRAVITY_HPP

#include <cmath>
#include "point.hpp"
#include "common.hpp"

namespace common {

    [[nodiscard]] inline point pair_force(double mass_i, double mass_j, point const & p_i, point const & p_j){ 
        // Function for the gravity formula: 
        // F_{ij} = G * \frac{m_{i} * m_{j}}{||p_{j} - p_{i}||^2} * \frac{p_{j} - p_{i}}{||p_{j} - p_{i}||}
         
        // Start by computing the difference of positions (p_{j} - p_{i}): 
        double const pj_pi_x = p_j.x - p_i.x; 
        double const pj_pi_y = p_j.y - p_i.y; 

        // Continue with the norm (distance) ||p_{j} - p_{i}||: 
        double const norm_pi_pj = std::sqrt(std::pow(pj_pi_x, 2) + std::pow(pj_pi_y, 2));

        // In case the distance is 0 because we are calculating the gravity force between a point and itself, 
        // We already know that this force will be 0. So, to avoid dividing by 0, we do the following: 
        if (norm_pi_pj == 0.0) {
            return {0.0, 0.0};
        }
        // Apply the formula to obtain the gravity force between two points, but only the constant part, not multiplying
        // by (p_{j} - p_{i}) just yet. Therefore, the variable will not be called f_ij, but factor.  

        double const factor = constants::G * mass_i * mass_j / std::pow(norm_pi_pj, 3);

        // Now, since multiplying a vector (in this case a point with two coordinates) by a scalar means to 
        // multiply each entry of the vector by said scalar, the final result is: 
        return {factor * pj_pi_x, factor * pj_pi_y}; 
        
    }
}

#endif