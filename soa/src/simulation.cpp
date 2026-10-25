#include "simulation.hpp"
#include "gravity.hpp"
#include <ranges>

std::vector<common::point> physics::accumulate_force_pairs(const ParticleSOA& particles) {
  const std::size_t n = particles.id.size();
  std::vector<common::point> force(n, {0.0, 0.0});

  // zip std::vector instances
  const auto particle_tuples = std::views::zip(particles.mass, particles.position, force);

  for (auto [i, particle_i] : particle_tuples | std::views::enumerate) {
    auto [m_i, p_i, f_i] = particle_i;

    double fx_i = 0.0;
    double fy_i = 0.0;

    for (auto [m_j, p_j, f_j] : particle_tuples | std::views::drop(i + 1))
    {
      const auto f = common::pair_force(m_i, m_j, p_i, p_j);
      f_j.x -= f.x; f_j.y -= f.y;
      fx_i += f.x; fy_i += f.y;
    }

    f_i.x += fx_i;
    f_i.y += fy_i;
  }
  return force;
}

std::vector<common::point> physics::compute_acceleration(const ParticleSOA & particles, const std::vector<common::point>& force) { 
  const std::size_t n = particles.id.size();
  std::vector<common::point> acceleration(n);
  
  const auto fma_tuple = std::views::zip(force, particles.mass, acceleration);
  
  for (auto [f,m,a] : fma_tuple) {
    const double mass_reciprocal = 1/m;
    a.x = f.x * mass_reciprocal;
    a.y = f.y * mass_reciprocal;
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

void update::all(ParticleSOA & particles, const std::vector<common::point>& acceleration, double dt) {
  // Expect vectors of the same size
  if (particles.position.size() != particles.velocity.size() or particles.position.size() != acceleration.size()) {
    throw std::invalid_argument("Mismatch in vector sizes.");
  }
  
  const auto pva_tuple = std::views::zip(particles.position, particles.velocity, acceleration);
  for (auto [p,v,a] : pva_tuple)
  {
    update_velocity(v, a, dt);
    update_position(p, v, dt);
  } 
}
