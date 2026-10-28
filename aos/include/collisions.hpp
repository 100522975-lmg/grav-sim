#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include "collision.hpp"
#include "particle.hpp"
#include <gsl/util> // gsl::at()
                    
namespace aos {
  [[nodiscard]] inline std::vector<common::CollisionPair> find_collisions(std::span<const particle> const particles) {
    std::size_t const n = particles.size();
    std::vector<common::CollisionPair> pairs;  
    pairs.reserve(static_cast<std::size_t>(static_cast<double>(n)*0.05));
    
    // find all colliding pairs in lexicographic order
    for (auto [i, p_i] : std::views::enumerate(particles)) {
      for (auto [j, p_j] : particles | std::views::enumerate | std::views::drop(i+1)) {
        if (common::are_colliding(p_i.position, p_i.radius, p_j.position, p_j.radius)) [[unlikely]] {
          pairs.emplace_back(i,j);
        }
      }
    }
    return pairs;
  }

  inline void resolve_collisions(std::span<particle> const particles) {
    const auto collisions = find_collisions(particles);
    for (const auto c : collisions) {
      auto p_i = particles.subspan(c.i,1).front();
      auto p_j = particles.subspan(c.j,1).front();
      common::collider const a{p_i.mass, p_i.radius, p_i.id};
      common::collider const b{p_j.mass, p_j.radius, p_j.id};
      if (common::choose_absorber(a,b)) {
        p_j.id = constants::UNUSED_ID;
        p_i.mass = p_i.mass + p_j.mass;
        p_i.velocity.x = p_i.velocity.x + p_j.velocity.x;
        p_i.velocity.y = p_i.velocity.y + p_j.velocity.y;
      } else {
        p_i.id = constants::UNUSED_ID;
        p_j.mass = p_j.mass + p_i.mass;
        p_j.velocity.x = p_j.velocity.x + p_i.velocity.x;
        p_j.velocity.y = p_j.velocity.y + p_i.velocity.y;
      }
    }
  }
}

#endif
