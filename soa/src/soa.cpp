#include "soa.hpp"

// Take Nx6 input matrix and construct SOA
ParticleSOA::ParticleSOA(const std::vector<std::vector<double>>& particles) {
  std::size_t n = particles.size();
  // Reserve space for each vector
  mass.reserve(n);
  radius.reserve(n);
  x_coord.reserve(n);
  y_coord.reserve(n);
  
  // Populate each vector with input data
  for (auto& row : particles) {
    mass.push_back(row[0]);
    radius.push_back(row[1]);
    x_coord.push_back(point{row[2], row[3]});
    y_coord.push_back(point{row[4], row[5]});
  }
}
