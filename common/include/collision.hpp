#ifndef COMMON_COLLISION_HPP
#define COMMON_COLLISION_HPP

#include <cstddef>

#include "point.hpp"

namespace common {
  // stores the indices of two colliding objects
  struct CollisionPair { std::size_t i, j; };

  // The properties of an object that decide which one absorbs the other
  struct collider {
    double mass;
    double radius;
    std::size_t id;
  };

  /// True if two objects collide: the distance between their positions is strictly less than the sum of their radii 
  [[nodiscard]] inline bool are_colliding(point const & pos_a, double radius_a, point const & pos_b,double radius_b) {
    double const dx = pos_a.x - pos_b.x;
    double const dy = pos_a.y - pos_b.y;
    double const radii_sum = radius_a + radius_b;
    return dx * dx + dy * dy < radii_sum * radii_sum;
  }

  /// Decides which object of a colliding pair absorbs the other.
  [[nodiscard]] inline bool choose_absorber(collider const & a, collider const & b) {
    if (a.mass != b.mass) {
      return a.mass > b.mass;
    }
    if (a.radius != b.radius) {
      return a.radius > b.radius;
    }
    return a.id < b.id;
  }

}  

#endif  
