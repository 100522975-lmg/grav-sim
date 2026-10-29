#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include "collision.hpp"
#include "soa.hpp"


namespace physics {
  /* Find collisions
   * Find all particle collisions in lexicographic order
   * Add colliding particles indices to a queue (vector)
   * Return the queue
   */
  [[nodiscard]] std::vector<common::CollisionPair> find_collisions(const ParticleSOA & particles);

  /* Resolve collisions
   * Obtain queue of colliding particle indices
   * Iterate over the queue and resolve in order
   * Skip match if any particle is marked as unused
   * Choose absorbing particle and update its fields
   */
  void resolve_collisions(ParticleSOA & particles);
}

#endif
