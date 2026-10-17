#include "soa.hpp"
#include "gravity.hpp"
#include <ranges>
#include <cmath>

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

std::vector<common::point> ParticleSOA::accumulate_force_pairs() const {
  const std::size_t n = id.size();
  std::vector<common::point> force(n);

  for (std::size_t i = 0; i < n; i++) {
    double fx_i = 0, fy_i = 0;
    for (std::size_t j = i+1; j < n; j++) {
      const auto f = common::pair_force(mass[i], mass[j], position[i], position[j]);
      // update force[j] based on Newton's third law
      force.at(j).x -= f.x; force.at(j).y -= f.y;
      // accumulate force[i]
      fx_i += f.x; fy_i += fy_i;
    }
    // update force[i]
    force.at(i).x += fx_i; force.at(i).y += fy_i;
  }

  return force;
}

std::vector<common::point> ParticleSOA::compute_acceleration(const std::vector<common::point>& force) const { 
  const std::size_t n = force.size();
  std::vector<common::point> acceleration(n);

  std::vector<double> inverse_mass(n);
  for (std::size_t i = 0; i < n; i++) {
    inverse_mass.at(i) = 1 / this->mass.at(i);
  }
  
  for (std::size_t j = 0; j < n; j++) {
    acceleration.at(j).x = inverse_mass.at(j) * force.at(j).x;
    acceleration.at(j).y = inverse_mass.at(j) * force.at(j).y;
  }

  return acceleration;

}

namespace {
  inline void update_velocity(common::point& v, common::point a, double dt) {
    v.x = v.x + a.x*dt;
    v.y = v.y + a.y*dt;
  }

  inline void update_position(common::point& p, common::point v, double dt) {
    p.x = p.x + v.x*dt;
    p.y = p.y + v.y*dt;
  }
}

void ParticleSOA::update_all(const std::vector<common::point>& acceleration, double dt) {
  // Expect vectors of the same size
  if (position.size() != velocity.size() || position.size() != acceleration.size()) {
    throw std::invalid_argument("Mismatch in vector sizes.");
  }

  for (auto&& [p,v,a] : std::views::zip(position, velocity, acceleration))
  {
    update_velocity(v, a, dt);
    update_position(p, v, dt);
  } 
}

void ParticleSOA::apply_rebound(const double width, const double height) {
  for (auto [p, v, r] : std::views::zip(position, velocity, radius)) {
    if (p.x - r <= 0) { p.x = r; v.x = std::abs(v.x); }
    else if (p.x + r >= width) { p.x = width - r; v.x = -1*std::abs(v.x); }

    if (p.y - r <= 0) { p.y = r; v.y = std::abs(v.y); }
    else if (p.y + r >= height) { p.y = height - r; v.y = -1*std::abs(v.y); }
  }
}
