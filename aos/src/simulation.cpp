#include "simulation.hpp"
#include "gravity.hpp"
#include "rebound.hpp"

#include <iterator>
#include <vector>

void reset_acceleration(std::vector<particle> & particles){
    for (auto & p : particles){
        p.acceleration=common::point{};
    }
}

void accumulate_pair_force(particle & a, particle & b){
    //Force that b exerts on a. (eq 1 and 2)
    common::point const force=common::pair_force(a.mass,b.mass,a.position,b.position);
    //a pulled towards b (add force)
    a.acceleration.x+=force.x;
    a.acceleration.y+=force.y;
    //b pulled toward a with same force but oppopsite direction
    b.acceleration.x-=force.x;
    b.acceleration.y-=force.y;
}

void accumulate_forces(std::vector<particle> & particles){
    //visit every pair exactly once
    for(auto first=particles.begin();first!=particles.end();++first){
        for(auto second=std::next(first);second!=particles.end();++second){
            accumulate_pair_force(*first,*second);
        }
    }
}

void forces_to_acceleration(std::vector<particle> & particles){
    for (auto & p:particles){
        //one division per particle (eq 3)
        double const inverse_mass=1.0/p.mass;
        p.acceleration.x*=inverse_mass;
        p.acceleration.y*=inverse_mass;
    }
}

void compute_acceleration(std::vector<particle> & particles){
    //forget previous iteration
    reset_acceleration(particles);
    //sum of the forces on each particle
    accumulate_forces(particles);
    //divide by masses
    forces_to_acceleration(particles);
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
        //eq 4
        update_velocity(p,time_step);
        //eq 5 with new velocity
        update_position(p,time_step);
        apply_rebound(p,width,height);
    }
}