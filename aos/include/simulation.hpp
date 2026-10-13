//Physics of one simulation iteration for AOS 

#ifndef AOS_SIMULATION_HPP
#define AOS_SIMULATION_HPP

#include <vector>

#include "particle.hpp"

//sets acceleration of every particle to 0
void reset_acceleration(std::vector<particle> & particles);

//adds to a and b the gravitational force between them
//a being the first particle of the pair and b being the second
void acummulate_forces(std::vector <particle> & particles);

//converts the accumulated force of every particle into the acceleration using eq 3
void forces_to_acceleration(std::vector <particle> & particles);

//computes the acceleration of every particle 
// (discardes previous values, rests, accumulates forces of all pairs and divides by the mass)
void compute_acceleration(std::vector <particle> & particles);

//update velocity usin eq 4
void update_velocity(particle & p, double time_step);

//updates position using eq 5
void update_position(particle & p, double time_step);

//applies rebound rule against the walls of the enclosure
void apply_rebound(particle & p,double width, double heigh)

//runs one simulation WITHOUT collisions
void step(std::vector<particle> & particles, double width, double height, double time_step);

#endif