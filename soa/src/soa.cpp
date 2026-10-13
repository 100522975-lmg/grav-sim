#include "soa.hpp"

// Take Nx6 input matrix and construct SOA
ParticleSOA::ParticleSOA(const std::vector<std::vector<double>>& particles) {
  std::size_t n = particles.size();
  // Reserve space for each vector
  id.reserve(n);
  mass.reserve(n);
  radius.reserve(n);
  position.reserve(n);
  velocity.reserve(n);

  // Populate each vector with input data
  for (int i = 0; auto& row : particles) {
    id.push_back(i++);
    mass.push_back(row.at(0));
    radius.push_back(row.at(1));
    position.push_back(common::point{row.at(2), row.at(3)});
    velocity.push_back(common::point{row.at(4), row.at(5)});
  }
}

std::vector<common::point> ParticleSOA::calculate_accelerations(const std::vector<std::vector<common::point>>& forces) { 
  const std::size_t n = forces.size();
  std::vector<common::point> accelerations(n);

  std::vector<double> inverse_mass(n);
  for (std::size_t i = 0; i < n; i++) {
    inverse_mass.at(i) = 1 / this->mass.at(i);
  }

  for (std::size_t i = 0; i < n; i++) {
    const auto& row = forces.at(i);
    common::point sum_i;
    for (std::size_t j = i+1; j < n; j++) {
      const common::point f = row.at(j);
      sum_i.x += f.x * inverse_mass.at(i);
      sum_i.y += f.y * inverse_mass.at(i);
      accelerations.at(j).x -= f.x * inverse_mass.at(j);
      accelerations.at(j).y -= f.y * inverse_mass.at(j);
    }
    accelerations.at(i).x += sum_i.x;
    accelerations.at(i).y += sum_i.y;
  }
  return accelerations;
}
