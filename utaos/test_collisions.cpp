#include "collisions.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <vector>

#include "particle.hpp"
#include "point.hpp"

using common::point;

namespace {


  struct particle_motion {
      point position;
      point velocity;
  };

  particle make_p(std::size_t id, double mass, double radius, particle_motion motion) {
      return particle{
          id,
          mass,
          radius,
          motion.position,
          motion.velocity,
          point{0.0, 0.0}
      };
  }
}  


// merge_particles tests


TEST(test_merge_particles, updates_mass_and_velocity_correctly) {
  particle absorber = make_p(0, 5.0, 2.0, {point{10.0, 10.0}, point{1.0, -2.0}});
  particle const absorbed = make_p(1, 3.0, 1.5, {point{10.5, 10.5}, point{4.0, 3.0}});

  aos::merge_particles(absorber, absorbed);

  // Mass and velocities should add up
  EXPECT_DOUBLE_EQ(absorber.mass, 8.0);
  EXPECT_DOUBLE_EQ(absorber.velocity.x, 5.0);
  EXPECT_DOUBLE_EQ(absorber.velocity.y, 1.0);
}

TEST(test_merge_particles, preserves_absorber_id_position_and_radius) {
  particle absorber = make_p(42, 10.0, 5.0, {point{12.0, 34.0}, point{1.0, 1.0}});
  particle const absorbed = make_p(99, 10.0, 2.0, {point{13.0, 35.0}, point{2.0, 2.0}});

  aos::merge_particles(absorber, absorbed);

  EXPECT_EQ(absorber.id, 42U);
  EXPECT_DOUBLE_EQ(absorber.position.x, 12.0);
  EXPECT_DOUBLE_EQ(absorber.position.y, 34.0);
  EXPECT_DOUBLE_EQ(absorber.radius, 5.0);
}

TEST(test_merge_particles, resets_acceleration_to_zero) {
  particle absorber = make_p(1, 2.0, 1.0, {point{0.0, 0.0}, point{0.0, 0.0}});
  absorber.acceleration = point{9.81, -4.5};
  particle const absorbed = make_p(2, 1.0, 1.0, {point{0.1, 0.1}, point{0.0, 0.0}});

  aos::merge_particles(absorber, absorbed);

  EXPECT_DOUBLE_EQ(absorber.acceleration.x, 0.0);
  EXPECT_DOUBLE_EQ(absorber.acceleration.y, 0.0);
}


// find_collision tests


TEST(test_find_collision, returns_nullopt_when_no_particles_collide) {
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{10.0, 0.0}, point{}}),
      make_p(2, 1.0, 1.0, {point{0.0, 10.0}, point{}})
  };

  auto const result = aos::find_collision(ps);
  EXPECT_FALSE(result.has_value());
}

TEST(test_find_collision, returns_nullopt_for_empty_or_single_particle) {
  std::vector<particle> const empty_ps;
  EXPECT_FALSE(aos::find_collision(empty_ps).has_value());

  std::vector<particle> const single_ps{make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}})};
  EXPECT_FALSE(aos::find_collision(single_ps).has_value());
}

TEST(test_find_collision, returns_first_colliding_pair_in_lexicographic_order) {
  // Pair (0, 2) collides (dist 1.0 < r1+r2=2.0)
  // Pair (1, 2) also collides (dist 1.0 < r1+r2=2.0)
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{0.0, 2.0}, point{}}),
      make_p(2, 1.0, 1.0, {point{0.0, 1.0}, point{}})
  };

  auto const result = aos::find_collision(ps);
  if (!result.has_value()) {
    FAIL() << "Expected a collision, but find_collision returned nullopt";
    return;
  }
  EXPECT_EQ(result->first, 0U);
  EXPECT_EQ(result->second, 2U);
}

TEST(test_find_collision, touching_at_exact_radius_distance_is_not_a_collision) {
  // Distance = 2.0, sum of radii = 1.0 + 1.0 = 2.0 -> NOT colliding
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{2.0, 0.0}, point{}})
  };

  EXPECT_FALSE(aos::find_collision(ps).has_value());
}


// resolve_collisions tests


TEST(test_resolve_collisions, higher_mass_absorbs_lower_mass) {
  // Particle 0: mass 10.0 (index 0)
  // Particle 1: mass 2.0 (index 1) - colliding
  std::vector<particle> ps{
      make_p(0, 10.0, 1.0, {point{0.0, 0.0}, point{1.0, 0.0}}),
      make_p(1, 2.0, 1.0, {point{0.5, 0.0}, point{0.0, 2.0}})
  };

  aos::resolve_collisions(ps);

  ASSERT_EQ(ps.size(), 1U);
  EXPECT_EQ(ps.at(0).id, 0U);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 12.0);
  EXPECT_DOUBLE_EQ(ps.at(0).position.x, 0.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, 2.0);
}

TEST(test_resolve_collisions, equal_mass_higher_radius_absorbs) {
  // Same mass (5.0), Particle 1 has larger radius (2.0 vs 1.0)
  std::vector<particle> ps{
      make_p(0, 5.0, 1.0, {point{0.0, 0.0}, point{1.0, 0.0}}),
      make_p(1, 5.0, 2.0, {point{0.5, 0.0}, point{0.0, 1.0}})
  };

  aos::resolve_collisions(ps);

  ASSERT_EQ(ps.size(), 1U);
  EXPECT_EQ(ps.at(0).id, 1U);  // Particle 1 absorbed Particle 0
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 10.0);
  EXPECT_DOUBLE_EQ(ps.at(0).radius, 2.0);
}

TEST(test_resolve_collisions, equal_mass_and_radius_smaller_id_absorbs) {
  // Same mass (5.0) and radius (1.0). Particle 3 vs Particle 7 -> Particle 3 absorbs
  std::vector<particle> ps{
      make_p(7, 5.0, 1.0, {point{0.0, 0.0}, point{0.0, 1.0}}),
      make_p(3, 5.0, 1.0, {point{0.5, 0.0}, point{1.0, 0.0}})
  };

  aos::resolve_collisions(ps);

  ASSERT_EQ(ps.size(), 1U);
  EXPECT_EQ(ps.at(0).id, 3U);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 10.0);
}

TEST(test_resolve_collisions, resolves_multiple_cascading_collisions) {
  // Three colliding particles at positions 0.0, 0.5, and 1.0
  std::vector<particle> ps{
      make_p(0, 10.0, 1.0, {point{0.0, 0.0}, point{1.0, 0.0}}),
      make_p(1, 2.0, 1.0, {point{0.5, 0.0}, point{0.0, 1.0}}),
      make_p(2, 3.0, 1.0, {point{1.0, 0.0}, point{0.0, 0.0}})
  };

  aos::resolve_collisions(ps);

  // All three should merge into Particle 0 (greatest mass)
  ASSERT_EQ(ps.size(), 1U);
  EXPECT_EQ(ps.at(0).id, 0U);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 15.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, 1.0);
}

TEST(test_resolve_collisions, no_op_when_no_collisions) {
  std::vector<particle> ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{10.0, 10.0}, point{}})
  };

  aos::resolve_collisions(ps);

  EXPECT_EQ(ps.size(), 2U);
  EXPECT_EQ(ps.at(0).id, 0U);
  EXPECT_EQ(ps.at(1).id, 1U);
}

TEST(test_resolve_collisions, empty_vector_handled_gracefully) {
  std::vector<particle> ps;
  EXPECT_NO_THROW(aos::resolve_collisions(ps));
  EXPECT_TRUE(ps.empty());
}