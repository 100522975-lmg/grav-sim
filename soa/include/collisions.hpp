#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include "collision.hpp"
#include "soa.hpp"


namespace physics {
  [[nodiscard]] std::vector<common::CollisionPair> find_collisions(const ParticleSOA & particles);
  void resolve_collisions(ParticleSOA & particles);
}

#endif
