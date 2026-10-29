//Physics of one simulation iteration for AOS 

#ifndef AOS_SIMULATION_HPP
#define AOS_SIMULATION_HPP

#include <vector>
#include <span>
#include <progargs.hpp>

#include "particle.hpp"

// Update velocity using eq. 4
inline void update_velocity(particle & p, double time_step){
    p.velocity.x+=p.acceleration.x*time_step;
    p.velocity.y+=p.acceleration.y*time_step;
}

// Update position using eq. 5
inline void update_position(particle & p, double time_step){
    p.position.x+=p.velocity.x*time_step;
    p.position.y+=p.velocity.y*time_step;
}

//runs one simulation WITHOUT collisions
void step(std::vector<particle> & particles, std::vector<common::point> & forces, std::vector<common::point> & accelerations, const common::program_arguments & progargs);

#endif
