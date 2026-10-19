#ifndef AOS_COLLISION_HPP
#define AOS_COLLISION_HPP

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

#include "collision.hpp"
#include "particle.hpp"

namespace aos {

  /// Combines 'absorbed' into 'absorber' 
  inline void merge_particles(particle & absorber, particle const & absorbed) {
    absorber.mass += absorbed.mass;
    // Radius and position remain those of the absorber
    absorber.velocity.x += absorbed.velocity.x;
    absorber.velocity.y += absorbed.velocity.y;
    // Acceleration will be recomputed in the next gravity pass
    absorber.acceleration = common::point{0.0, 0.0};
  }

  /// Scans particle pairs in lexicographic order of their identifiers.
  /// Returns indices (i, j) of the first colliding pair found, or std::nullopt.
  [[nodiscard]] inline std::optional<std::pair<std::size_t, std::size_t>>
  find_collision(std::vector<particle> const & particles) {
    std::size_t const n = particles.size();
    for (std::size_t i = 0; i < n; ++i) {
      for (std::size_t j = i + 1; j < n; ++j) {
        if (common::are_colliding(particles.at(i).position, particles.at(i).radius,
                                  particles.at(j).position, particles.at(j).radius)) {
          return std::make_pair(i, j);
        }
      }
    }
    return std::nullopt;
  }

  /// Resolves all collisions in the system.
  /// Restarts scan from the lowest-identifier remaining object after each resolution.
  inline void resolve_collisions(std::vector<particle> & particles) {
    while (true) {
      auto const collision = find_collision(particles);
      if (!collision.has_value()) {
        break; 
      }

      auto const [idx_a, idx_b] = *collision;

      common::collider const col_a{particles.at(idx_a).mass, particles.at(idx_a).radius, particles.at(idx_a).id};
      common::collider const col_b{particles.at(idx_b).mass, particles.at(idx_b).radius, particles.at(idx_b).id};

      if (common::choose_absorber(col_a, col_b)) {
        // Particle A absorbs Particle B
        merge_particles(particles.at(idx_a), particles.at(idx_b));
        particles.erase(particles.begin() + static_cast<std::ptrdiff_t>(idx_b));
      } else {
        // Particle B absorbs Particle A
        merge_particles(particles.at(idx_b), particles.at(idx_a));
        particles.erase(particles.begin() + static_cast<std::ptrdiff_t>(idx_a));
      }
    // After each merge, the vector is modified, so we restart the search for collisions from the beginning.
    }
  }

} // namespace aos

#endif 