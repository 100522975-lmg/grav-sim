#include "simulation.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <string>
#include <vector>

#include "common.hpp"
#include "particle.hpp"
#include "point.hpp"
#include "progargs.hpp"

using common::point;
using constants::G;

namespace {

constexpr double tolerance = 1e-24;

// Creates a particle with radius 1 and zero acceleration.
particle make(std::size_t id, double mass, point pos, point vel) {
  return particle{id, mass, 1.0, pos, vel, point{}};
}

// Builds the program arguments that step() reads: width, height and time step.
common::program_arguments make_args(double width, double height, double dt) {
  std::vector<std::string> const args{
      "grav-aos", "1", std::to_string(width), std::to_string(height), std::to_string(dt)};
  return common::program_arguments{args};
}

// step() takes the force/acceleration buffers from the caller (main reuses them
// across iterations). It clears and refills them itself, so tests start empty.
struct buffers {
  std::vector<point> forces;
  std::vector<point> accelerations;
};

}  // namespace

// update_velocity

TEST(test_update_velocity, adds_acceleration_times_time_step) {
  particle p = make(0, 1.0, point{}, point{1.0, 2.0});
  p.acceleration = point{2.0, -1.0};

  update_velocity(p, 0.5);

  EXPECT_DOUBLE_EQ(p.velocity.x, 2.0);
  EXPECT_DOUBLE_EQ(p.velocity.y, 1.5);
}

TEST(test_update_velocity, zero_acceleration_keeps_the_velocity) {
  particle p = make(0, 1.0, point{}, point{3.0, -4.0});

  update_velocity(p, 10.0);

  EXPECT_DOUBLE_EQ(p.velocity.x, 3.0);
  EXPECT_DOUBLE_EQ(p.velocity.y, -4.0);
}

TEST(test_update_velocity, does_not_touch_the_other_fields) {
  particle p = make(4, 2.0, point{1.0, 2.0}, point{3.0, 4.0});
  p.acceleration = point{1.0, 1.0};

  update_velocity(p, 1.0);

  EXPECT_EQ(p.id, 4U);
  EXPECT_DOUBLE_EQ(p.mass, 2.0);
  EXPECT_DOUBLE_EQ(p.radius, 1.0);
  EXPECT_DOUBLE_EQ(p.position.x, 1.0);
  EXPECT_DOUBLE_EQ(p.position.y, 2.0);
  EXPECT_DOUBLE_EQ(p.acceleration.x, 1.0);
  EXPECT_DOUBLE_EQ(p.acceleration.y, 1.0);
}

// update_position

TEST(test_update_position, adds_velocity_times_time_step) {
  particle p = make(0, 1.0, point{1.0, 1.0}, point{2.0, -4.0});

  update_position(p, 0.5);

  EXPECT_DOUBLE_EQ(p.position.x, 2.0);
  EXPECT_DOUBLE_EQ(p.position.y, -1.0);
}

TEST(test_update_position, zero_velocity_keeps_the_position) {
  particle p = make(0, 1.0, point{7.0, 8.0}, point{});

  update_position(p, 5.0);

  EXPECT_DOUBLE_EQ(p.position.x, 7.0);
  EXPECT_DOUBLE_EQ(p.position.y, 8.0);
}

TEST(test_update_position, does_not_touch_the_velocity) {
  particle p = make(0, 1.0, point{1.0, 1.0}, point{2.0, -4.0});

  update_position(p, 0.5);

  EXPECT_DOUBLE_EQ(p.velocity.x, 2.0);
  EXPECT_DOUBLE_EQ(p.velocity.y, -4.0);
}

// step: free motion
// accumulate_forces, compute_acceleration and apply_rebound are internal to
// simulation.cpp now, so they are tested through step().

TEST(test_step, no_particles_is_fine) {
  std::vector<particle> ps;
  buffers b;

  EXPECT_NO_THROW(step(ps, b.forces, b.accelerations, make_args(100.0, 100.0, 0.1)));
  EXPECT_TRUE(ps.empty());
}

TEST(test_step, isolated_particle_moves_in_a_straight_line) {
  std::vector<particle> ps{make(0, 1.0, {5.0, 5.0}, {1.0, 2.0})};
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(100.0, 100.0, 0.5));

  EXPECT_NEAR(ps.at(0).position.x, 5.5, 1e-9);
  EXPECT_NEAR(ps.at(0).position.y, 6.0, 1e-9);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, 2.0);
}

TEST(test_step, isolated_particle_has_zero_acceleration) {
  std::vector<particle> ps{make(0, 2.0, {5.0, 5.0}, {})};
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(100.0, 100.0, 0.5));

  ASSERT_EQ(b.accelerations.size(), 1U);
  EXPECT_DOUBLE_EQ(b.accelerations.at(0).x, 0.0);
  EXPECT_DOUBLE_EQ(b.accelerations.at(0).y, 0.0);
}

// step: gravity

TEST(test_step, gravity_matches_the_hand_computed_value) {
  // Equal masses separated by 100 units: a = G * m / d^2 = 66.743 (approx).
  std::vector<particle> ps{
      make(0, 1.0e16, {100.0, 500.0}, {0.0, 0.0}),
      make(1, 1.0e16, {200.0, 500.0}, {0.0, 0.0}),
  };
  buffers b;
  constexpr double dt = 1.0e-6;

  step(ps, b.forces, b.accelerations, make_args(1000.0, 1000.0, dt));

  double const expected = G * 1.0e16 / (100.0 * 100.0) * dt;
  EXPECT_NEAR(ps.at(0).velocity.x, expected, 1e-12);
  EXPECT_NEAR(ps.at(1).velocity.x, -expected, 1e-12);
  EXPECT_NEAR(ps.at(0).velocity.y, 0.0, 1e-12);
  EXPECT_NEAR(ps.at(1).velocity.y, 0.0, 1e-12);
}

TEST(test_step, particles_attract_each_other) {
  std::vector<particle> ps{
      make(0, 1.0e16, {100.0, 100.0}, {}),
      make(1, 1.0e16, {200.0, 100.0}, {}),
  };
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(1000.0, 1000.0, 1.0e-3));

  EXPECT_GT(ps.at(0).velocity.x, 0.0);
  EXPECT_LT(ps.at(1).velocity.x, 0.0);
  EXPECT_GT(ps.at(0).position.x, 100.0);
  EXPECT_LT(ps.at(1).position.x, 200.0);
}

TEST(test_step, acceleration_is_force_divided_by_own_mass) {
  // Distance 5, direction (0.6, 0.8). Masses 2 and 5: each feels the same
  // force, but acceleration scales with 1/mass.
  std::vector<particle> ps{
      make(0, 2.0, {0.0, 0.0}, {}),
      make(1, 5.0, {3.0, 4.0}, {}),
  };
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(1000.0, 1000.0, 0.001));

  double const force = G * 2.0 * 5.0 / 25.0;
  ASSERT_EQ(b.accelerations.size(), 2U);
  EXPECT_NEAR(b.accelerations.at(0).x, force * 0.6 / 2.0, tolerance);
  EXPECT_NEAR(b.accelerations.at(0).y, force * 0.8 / 2.0, tolerance);
  EXPECT_NEAR(b.accelerations.at(1).x, -force * 0.6 / 5.0, tolerance);
  EXPECT_NEAR(b.accelerations.at(1).y, -force * 0.8 / 5.0, tolerance);
}

TEST(test_step, forces_are_equal_and_opposite) {
  std::vector<particle> ps{
      make(0, 2.0, {0.0, 0.0}, {}),
      make(1, 5.0, {3.0, 4.0}, {}),
  };
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(1000.0, 1000.0, 0.001));

  ASSERT_EQ(b.forces.size(), 2U);
  EXPECT_NEAR(b.forces.at(0).x, G * 2.0 * 5.0 / 25.0 * 0.6, tolerance);
  EXPECT_DOUBLE_EQ(b.forces.at(1).x, -b.forces.at(0).x);
  EXPECT_DOUBLE_EQ(b.forces.at(1).y, -b.forces.at(0).y);
}

TEST(test_step, three_particles_add_both_forces_on_the_middle_one) {
  std::vector<particle> ps{
      make(0, 1.0, {10.0, 50.0}, {}),
      make(1, 1.0, {11.0, 50.0}, {}),
      make(2, 3.0, {13.0, 50.0}, {}),
  };
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(1000.0, 1000.0, 0.001));

  double const from_first = -G;
  double const from_third = G * 1.0 * 3.0 / 4.0;
  ASSERT_EQ(b.forces.size(), 3U);
  EXPECT_NEAR(b.forces.at(1).x, from_first + from_third, tolerance);
}

TEST(test_step, total_force_of_the_system_is_zero) {
  std::vector<particle> ps{
      make(0, 2.0, {20.0, 20.0}, {}),
      make(1, 3.0, {24.0, 21.0}, {}),
      make(2, 5.0, {18.0, 27.0}, {}),
  };
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(1000.0, 1000.0, 0.001));

  ASSERT_EQ(b.forces.size(), 3U);
  double total_x = 0.0;
  double total_y = 0.0;
  for (auto const& f : b.forces) {
    total_x += f.x;
    total_y += f.y;
  }
  EXPECT_NEAR(total_x, 0.0, tolerance);
  EXPECT_NEAR(total_y, 0.0, tolerance);
}

TEST(test_step, middle_particle_between_equal_masses_is_balanced) {
  std::vector<particle> ps{
      make(0, 4.0, {100.0, 100.0}, {}),
      make(1, 1.0, {105.0, 100.0}, {}),
      make(2, 4.0, {110.0, 100.0}, {}),
  };
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(1000.0, 1000.0, 0.001));

  ASSERT_EQ(b.accelerations.size(), 3U);
  EXPECT_NEAR(b.accelerations.at(1).x, 0.0, tolerance);
  EXPECT_GT(b.accelerations.at(0).x, 0.0);
  EXPECT_LT(b.accelerations.at(2).x, 0.0);
}

// step: ordering inside one iteration

TEST(test_step, position_uses_the_already_updated_velocity) {
  std::vector<particle> ps{
      make(0, 1.0e16, {500.0, 500.0}, {1.0, 0.0}),
      make(1, 1.0e16, {600.0, 500.0}, {0.0, 0.0}),
  };
  buffers b;
  double const dt = 0.1;

  step(ps, b.forces, b.accelerations, make_args(1000.0, 1000.0, dt));

  // Velocity changed by gravity first, then the position moved with it.
  EXPECT_GT(ps.at(0).velocity.x, 1.0);
  EXPECT_NEAR(ps.at(0).position.x, 500.0 + ps.at(0).velocity.x * dt, 1e-9);
  EXPECT_NEAR(ps.at(1).position.x, 600.0 + ps.at(1).velocity.x * dt, 1e-9);
}

TEST(test_step, all_forces_use_the_positions_of_the_start_of_the_iteration) {
  std::vector<particle> ps{
      make(0, 1.0e16, {100.0, 500.0}, {}),
      make(1, 1.0e16, {200.0, 500.0}, {}),
  };
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(1000.0, 1000.0, 1.0e-3));

  // If particle 1 saw particle 0 already moved, the two speeds would differ.
  EXPECT_DOUBLE_EQ(ps.at(1).velocity.x, -ps.at(0).velocity.x);
}

TEST(test_step, reusing_the_buffers_does_not_accumulate_old_forces) {
  std::vector<particle> first{
      make(0, 1.0e16, {100.0, 500.0}, {}),
      make(1, 1.0e16, {200.0, 500.0}, {}),
  };
  auto second = first;
  buffers reused;
  buffers fresh;
  auto const args = make_args(1000.0, 1000.0, 1.0e-6);

  step(first, reused.forces, reused.accelerations, args);
  first = second;  // same starting state again, but the buffers are dirty
  step(first, reused.forces, reused.accelerations, args);
  step(second, fresh.forces, fresh.accelerations, args);

  EXPECT_DOUBLE_EQ(first.at(0).velocity.x, second.at(0).velocity.x);
  EXPECT_DOUBLE_EQ(first.at(1).velocity.x, second.at(1).velocity.x);
  ASSERT_EQ(reused.forces.size(), 2U);
  EXPECT_DOUBLE_EQ(reused.forces.at(0).x, fresh.forces.at(0).x);
}

// step: rebound (enclosure width x height, particle radius = 1)

TEST(test_step, inside_the_enclosure_nothing_rebounds) {
  std::vector<particle> ps{make(0, 1.0, {5.0, 5.0}, {-1.0, 2.0})};
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(10.0, 10.0, 0.1));

  EXPECT_NEAR(ps.at(0).position.x, 4.9, 1e-9);
  EXPECT_NEAR(ps.at(0).position.y, 5.2, 1e-9);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, -1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, 2.0);
}

TEST(test_step, left_wall_flips_x_only) {
  // Moves to x = 0.5, inside the wall by the radius: clamped to 1.0.
  std::vector<particle> ps{make(0, 1.0, {1.5, 5.0}, {-1.0, 3.0})};
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(10.0, 10.0, 1.0));

  EXPECT_DOUBLE_EQ(ps.at(0).position.x, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).position.y, 8.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, 3.0);
}

TEST(test_step, right_wall_uses_the_width) {
  std::vector<particle> ps{make(0, 1.0, {8.5, 5.0}, {1.0, 0.0})};
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(10.0, 20.0, 1.0));

  EXPECT_DOUBLE_EQ(ps.at(0).position.x, 9.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, -1.0);
}

TEST(test_step, top_wall_uses_the_height_not_the_width) {
  std::vector<particle> ps{make(0, 1.0, {5.0, 18.5}, {1.0, 1.0})};
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(10.0, 20.0, 1.0));

  EXPECT_DOUBLE_EQ(ps.at(0).position.y, 19.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, -1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, 1.0);
}

TEST(test_step, bottom_wall_flips_y_only) {
  std::vector<particle> ps{make(0, 1.0, {5.0, 1.3}, {1.0, -0.5})};
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(10.0, 10.0, 1.0));

  EXPECT_DOUBLE_EQ(ps.at(0).position.y, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, 0.5);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, 1.0);
}

TEST(test_step, corner_flips_both_axes) {
  std::vector<particle> ps{make(0, 1.0, {1.5, 8.5}, {-1.0, 1.0})};
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(10.0, 10.0, 1.0));

  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, -1.0);
}

TEST(test_step, exactly_touching_a_wall_counts_as_a_rebound) {
  std::vector<particle> ps{make(0, 1.0, {2.0, 5.0}, {-1.0, 0.0})};
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(10.0, 10.0, 1.0));

  EXPECT_DOUBLE_EQ(ps.at(0).position.x, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, 1.0);
}

TEST(test_step, every_particle_rebounds_independently) {
  std::vector<particle> ps{
      make(0, 1.0, {1.5, 5.0}, {-1.0, 0.0}),
      make(1, 1.0, {5.0, 5.0}, {1.0, 0.0}),
  };
  buffers b;

  step(ps, b.forces, b.accelerations, make_args(10.0, 10.0, 1.0));

  // The two unit masses also attract each other (~1e-11), hence EXPECT_NEAR.
  EXPECT_NEAR(ps.at(0).velocity.x, 1.0, 1e-9);   // bounced off the left wall
  EXPECT_NEAR(ps.at(1).velocity.x, 1.0, 1e-9);   // untouched
  EXPECT_NEAR(ps.at(1).position.x, 6.0, 1e-9);
}