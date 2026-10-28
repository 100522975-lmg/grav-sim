#include "collisions.hpp"
#include <vector>
#include <ranges>

std::vector<common::CollisionPair> physics::find_collisions(const ParticleSOA & particles) {
  // Estimate collision rate r=0.05 to preallocate size 
  const std::size_t n = particles.id.size();
  std::vector<common::CollisionPair> matches;
  matches.reserve(static_cast<std::size_t>(static_cast<double>(n)*0.05));
  const auto all = std::views::zip(
      std::views::iota(std::size_t{0}, n),
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
  for (const auto c : collisions) {
    // choose absorber
    const std::size_t i = c.i;
    auto & id_i = particles.id.at(i);
    auto & m_i = particles.mass.at(i);
    auto & v_i = particles.velocity.at(i);
    const auto & r_i = particles.radius.at(i);

    const std::size_t j = c.j;
    auto & id_j = particles.id.at(j);
    auto & m_j = particles.mass.at(j);
    auto & v_j = particles.velocity.at(j);
    const auto & r_j = particles.radius.at(j);

    if (id_i == constants::UNUSED_ID or id_j == constants::UNUSED_ID) { continue; }
    const common::collider a {m_i, r_i, id_i};
    const common::collider b {m_j, r_j, id_j};
    if (common::choose_absorber(a,b)) {
      id_j = constants::UNUSED_ID;
      m_i = m_i + m_j;
      v_i.x = v_i.x + v_j.x;
      v_i.y = v_i.y + v_j.y;
    } else {
      id_i = constants::UNUSED_ID;
      m_j = m_j + m_i;
      v_j.x = v_j.x + v_i.x;
      v_j.y = v_j.y + v_i.y;
    }
  }
}
