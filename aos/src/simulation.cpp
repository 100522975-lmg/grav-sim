#include "simulation.hpp"
#include "rebound.hpp"

#include <vector>

void reset_acceleration(std::vector<particle> & particles){
    for (auto & p : particles){
        p.acceleration=common::point{};
    }
}


void accumulate_forces(std::vector<particle> & particles){
    const std::size_t n = particles.size();
    //visit every pair exactly once
    for(std::size_t i=0; i<n;++i){
        for(std::size_t j=i+1;j<n;++j){
            accumulate_pair_force(particles.at(i),particles.at(j));
        }
    }
}



void compute_acceleration(std::vector<particle> & particles){
    //forget previous iteration
    reset_acceleration(particles);
    //sum of the forces on each particle
    accumulate_forces(particles);

}



void apply_rebound(particle & p, double width, double height){
    //rebound_effect checks both axes at onces and updates velocity and position
    common::rebound_effect(p.position,p.velocity,p.radius,common::point{width,height});
}

void step(std::vector<particle> & particles, double width, double height, double time_step ){
    //all accelerations first
    compute_acceleration(particles);
    //move every particle 
    for(auto & p:particles){
        update_velocity(p,time_step);
        //eq 5 with new velocity
        update_position(p,time_step);
        apply_rebound(p,width,height);
    }
}