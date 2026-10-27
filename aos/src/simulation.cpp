#include "simulation.hpp"
#include "rebound.hpp"

#include <vector>

void reset_acceleration(std::vector<particle> & particles){
    for (auto & p : particles){
        p.acceleration.x=0.0;
        p.acceleration.y=0.0;
    }
}


void accumulate_forces(std::vector<particle> & particles){
    const std::size_t n = particles.size();
    //visit every pair exactly once
    for(std::size_t i=0; i<n;++i){
        auto & p_i=particles.at(i);
        const double inv_m_i = 1.0/p_i.mass;
        for(std::size_t j=i+1;j<n;++j){
            auto & p_j=particles.at(j);
            
            //compute force exerted on p_i by p_j
            common::point const force=common::pair_force(p_i.mass,p_j.mass,p_i.position,p_j.position);
            //convert force to acceleration p_i
            p_i.acceleration.x+=force.x*inv_m_i;
            p_i.acceleration.y+=force.y*inv_m_i;
            //p_j acceleration
            const double inv_m_j=1.0/p_j.mass;
            p_j.acceleration.x+=force.x*inv_m_j;
            p_j.acceleration.y+=force.y*inv_m_j;
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