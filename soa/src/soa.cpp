#include "soa.hpp"
#include <algorithm>
#include <ranges>

// Remove unused particles from the end
void ParticleSOA::remove_unused() {
  if (id.empty()) { return; }

  const auto zipped = std::views::zip(id, mass, radius, position, velocity);
  const auto new_end = std::ranges::remove_if(zipped, [](const auto& particle_tuple) {
  const auto& [current_id, mass, radius, position, velocity] = particle_tuple;
    return current_id == unused_id;
  }).begin();

  const auto new_size = static_cast<std::size_t>(std::distance(zipped.begin(), new_end));

  id.resize(new_size);
  mass.resize(new_size);
  radius.resize(new_size);
  position.resize(new_size);
  velocity.resize(new_size);
}

// Take Nx6 input matrix and construct SOA
ParticleSOA::ParticleSOA(const std::vector<std::vector<double>>& particles) {
  std::size_t const n = particles.size();
  // Reserve space for each vector
  id.reserve(n);
  mass.reserve(n);
  radius.reserve(n);
  position.reserve(n);
  velocity.reserve(n);

  // Populate each vector with input data
  for (std::size_t i = 0; const auto& row : particles) {
    if (row.empty()) { i++; continue; } // skip bad particle
    id.push_back(i++);
    mass.push_back(row.at(0));
    radius.push_back(row.at(1));
    position.push_back(common::point{row.at(2), row.at(3)});
    velocity.push_back(common::point{row.at(4), row.at(5)});
  }
}


