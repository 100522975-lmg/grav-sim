//Physics of one simulation iteration for AOS 

#ifndef AOS_SIMULATION_HPP
#define AOS_SIMULATION_HPP

#include <vector>

#include "particle.hpp"
#include "gravity.hpp"

//sets acceleration of every particle to 0
void reset_acceleration(std::vector<particle> & particles);



//accumulates the force of every pair of particles
void accumulate_forces(std::vector <particle> & particles);


//computes the acceleration of every particle 
// (discardes previous values, rests, accumulates forces of all pairs and divides by the mass)
void compute_acceleration(std::vector <particle> & particles);

//update velocity usin eq 4
inline void update_velocity(particle & p, double time_step){
    p.velocity.x+=p.acceleration.x*time_step;
    p.velocity.y+=p.acceleration.y*time_step;
}
//updates position using eq 5
inline void update_position(particle & p, double time_step){
    p.position.x+=p.velocity.x*time_step;
    p.position.y+=p.velocity.y*time_step;
}
//applies rebound rule against the walls of the enclosure
void apply_rebound(particle & p,double width, double height);

//runs one simulation WITHOUT collisions
void step(std::vector<particle> & particles, double width, double height, double time_step);

#endif