#include "soa.hpp"

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
  for (int i = 0; const auto& row : particles) {
    id.push_back(i++);
    mass.push_back(row.at(0));
    radius.push_back(row.at(1));
    position.push_back(common::point{row.at(2), row.at(3)});
    velocity.push_back(common::point{row.at(4), row.at(5)});
  }
}


