#ifndef COMMON_REBOUND_HPP
#define COMMON_REBOUND_HPP

#include <cmath>
#include "point.hpp"
#include "common.hpp"

namespace common {
    inline void rebound_effect(point& p, point& velocity, double r, point dimensions){

        // There are several cases depending on the border that the ball is about to collide with. 

        // Case 1: left or rigth borders (x coordinate)
        if (p.x - r <= 0){ //left
            p.x = r; // update position
            velocity.x = std::abs(velocity.x); // update velocity
        } 
            
        else if (p.x + r >= dimensions.x){ //right
            p.x = dimensions.x - r; // update position
            velocity.x = -std::abs(velocity.x); // update velocity
        }
        // Case 3: upper or lower border (y coordinate)
        if (p.y - r <= 0) {// upper
            p.y = r; 
            velocity.y = std::abs(velocity.y); // update velocity
        }
        else if (p.y + r >= dimensions.y){ // lower
            p.y = dimensions.y - r; 
            velocity.y = -std::abs(velocity.y); // update velocity
        }

    }
}

#endif