#include "collisions.hpp"
#include <vector>
#include <ranges>

std::vector<CollisionPair> physics::find_collisions(const ParticleSOA & particles) {
  // Estimate collision rate r=0.05 to preallocate size 
  std::vector<CollisionPair> matches;
  const auto all = std::views::zip(
      std::views::iota(std::size_t{0}, particles.radius.size()),
      particles.radius, 
      particles.position);

  for (const auto [i, r_i, p_i] : all) {
    for (const auto [j, r_j, p_j] : all | std::views::drop(i+1)) {
      if (common::are_colliding(p_i, r_i, p_j, r_j)) [[unlikely]] {
        matches.emplace_back (i,j); 
      }
    }
  }
  return matches;
}

void physics::resolve_collisions(ParticleSOA & particles) {
  const auto collisions = find_collisions(particles);
  if (collisions.empty()) return; // exit early if no collisions
  auto * id = particles.id.data();
  auto * mass = particles.mass.data();
  auto * radius = particles.radius.data();
  auto * velocity = particles.velocity.data();
  for (const auto c : collisions) {
    // choose absorber
    std::size_t i = c.i;
    std::size_t j = c.j;
    if (*(id+i) == unused_id || *(id+j) == unused_id) continue;
    const common::collider a {*(mass+i), *(radius+i), i};
    const common::collider b {*(mass+j), *(radius+j), j};
    if (common::choose_absorber(a,b)) {
      *(id+j) = unused_id;
      *(mass+i) = *(mass+i) + *(mass+j);
      (velocity+i)->x = (velocity+i)->x + (velocity+j)->x;
      (velocity+i)->y = (velocity+i)->y + (velocity+j)->y;
    } else {
      *(id+i) = unused_id;
      *(mass+j) = *(mass+j) + *(mass+i);
      (velocity+j)->x = (velocity+j)->x + (velocity+i)->x;
      (velocity+j)->y = (velocity+j)->y + (velocity+i)->y;
    }
  }
}
