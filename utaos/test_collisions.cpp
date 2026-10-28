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

}  // namespace



// find_collisions tests

TEST(test_find_collisions, returns_empty_when_no_particles_collide) {
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{10.0, 0.0}, point{}}),
      make_p(2, 1.0, 1.0, {point{0.0, 10.0}, point{}})
  };

  const auto result = aos::find_collisions(ps);
  EXPECT_TRUE(result.empty());
}

TEST(test_find_collisions, returns_empty_for_empty_or_single_particle) {
  std::vector<particle> const empty_ps;
  EXPECT_TRUE(aos::find_collisions(empty_ps).empty());

  std::vector<particle> const single_ps{make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}})};
  EXPECT_TRUE(aos::find_collisions(single_ps).empty());
}

TEST(test_find_collisions, returns_first_colliding_pair_in_lexicographic_order) {
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{0.0, 2.0}, point{}}),
      make_p(2, 1.0, 1.0, {point{0.0, 0.05}, point{}})
  };

  const auto result = aos::find_collisions(ps);
  if (result.empty()) {
    FAIL() << "Expected a collision, but find_collisions returned empty vector";
    return;
  }
  EXPECT_EQ(result.at(0).i, 0U);
  EXPECT_EQ(result.at(0).j, 2U);
}

TEST(test_find_collisions, touching_at_exact_radius_distance_is_not_a_collision) {
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{2.0, 0.0}, point{}})
  };

  EXPECT_TRUE(aos::find_collisions(ps).empty());
}

// resolve_collisions tests

TEST(test_resolve_collisions, higher_mass_absorbs_lower_mass) {
  std::vector<particle> ps{
      make_p(0, 10.0, 1.0, {point{0.0, 0.0}, point{1.0, 0.0}}),
      make_p(1, 2.0, 1.0, {point{0.05, 0.0}, point{0.0, 2.0}})
  };

  aos::resolve_collisions(ps);

  ASSERT_EQ(ps.size(), 1U);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 12.0);
  EXPECT_DOUBLE_EQ(ps.at(0).position.x, 0.0);
}

TEST(test_resolve_collisions, resolves_multiple_cascading_collisions) {
  std::vector<particle> ps{
      make_p(0, 10.0, 1.0, {point{0.0, 0.0}, point{1.0, 0.0}}),
      make_p(1, 2.0, 1.0, {point{0.05, 0.0}, point{0.0, 1.0}}),
      make_p(2, 3.0, 1.0, {point{0.08, 0.0}, point{0.0, 0.0}})
  };

  aos::resolve_collisions(ps);

  ASSERT_EQ(ps.size(), 1U);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 15.0);
}

TEST(test_resolve_collisions, no_op_when_no_collisions) {
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{10.0, 10.0}, point{}})
  };

  std::vector<particle> ps_copy = ps;
  aos::resolve_collisions(ps_copy);

  EXPECT_EQ(ps_copy.size(), 2U);
}

TEST(test_resolve_collisions, empty_vector_handled_gracefully) {
  std::vector<particle> ps;
  EXPECT_NO_THROW(aos::resolve_collisions(ps));
  EXPECT_TRUE(ps.empty());
}