#ifndef AOS_PARTICLE_HPP
#define AOS_PARTICLE_HPP

#include <vector>
#include <ranges>

#include "point.hpp"
#include "common.hpp"

//particle struct containing 6 element. The id (asigned once), mass(>0), radius (>0), 
// position (x,y) of centre, velocity (vx,vy), acceleration (ax,ay).
struct particle {
    std::size_t id{};
    double mass{};
    double radius{};
    common::point position;
    common::point velocity;
    common::point acceleration;
};

inline particle make_particle(std::vector<double> const & row, std::size_t id){
    //since the row layout is {mass,radius,x,y,vx,vy}
    common::point const position{row.at(2),row.at(3)};
    common::point const velocity{row.at(4),row.at(5)};
    //acceleration starts at (0,0) and is recomputed every iteration
    return particle{id,row.at(0),row.at(1),position,velocity,common::point{}};
}

inline void remove_unused(std::vector<particle> & particles) {
    std::erase_if(particles, [](particle const& p) {
        return p.id == constants::UNUSED_ID;
    });    
}

#endif
