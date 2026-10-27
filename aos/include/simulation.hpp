//Physics of one simulation iteration for AOS 

#ifndef AOS_SIMULATION_HPP
#define AOS_SIMULATION_HPP

#include <vector>

#include "particle.hpp"
#include "gravity.hpp"

//sets acceleration of every particle to 0
void reset_acceleration(std::vector<particle> & particles);

//adds to a and b the gravitational force between them
//a being the first particle of the pair and b being the second
inline void accumulate_pair_force(particle & a, particle & b){
    //Force that b exerts on a. (eq 1 and 2)
    common::point const force=common::pair_force(a.mass,b.mass,a.position,b.position);
    //direct acceleration calculation  saves a secondary vector traversal loop
    double const inv_mass_a=1.0/a.mass;
    double const inv_mass_b=1.0/b.mass;
    //a pulled towards b (add force)
    a.acceleration.x+=force.x*inv_mass_a;
    a.acceleration.y+=force.y*inv_mass_a;
    //b pulled toward a with same force but oppopsite direction
    b.acceleration.x-=force.x*inv_mass_b;
    b.acceleration.y-=force.y*inv_mass_b;
}

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