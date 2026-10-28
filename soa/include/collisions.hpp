#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include "collision.hpp"
#include "soa.hpp"

struct CollisionPair { std::size_t i, j; };

namespace physics {
  [[nodiscard]] std::vector<CollisionPair> find_collisions(const ParticleSOA & particles);
  void resolve_collisions(ParticleSOA & particles);
}

#endif
