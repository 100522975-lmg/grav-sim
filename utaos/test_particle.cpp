#include "particle.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "common.hpp"

namespace {

// Particle with only the id and mass filled in; enough for remove_unused.
particle make_simple(std::size_t id, double mass) {
  return particle{id, mass, 1.0, {}, {}, {}};
}

}  // namespace

// make_particle

TEST(test_make_particle, fills_every_field_from_the_row) {
  // Row layout: {mass, radius, x, y, vx, vy}
  particle const p = make_particle({2.0, 0.5, 3.0, 4.0, -1.0, 6.0}, 7);
  EXPECT_EQ(p.id, 7U);
  EXPECT_DOUBLE_EQ(p.mass, 2.0);
  EXPECT_DOUBLE_EQ(p.radius, 0.5);
  EXPECT_DOUBLE_EQ(p.position.x, 3.0);
  EXPECT_DOUBLE_EQ(p.position.y, 4.0);
  EXPECT_DOUBLE_EQ(p.velocity.x, -1.0);
  EXPECT_DOUBLE_EQ(p.velocity.y, 6.0);
}

TEST(test_make_particle, acceleration_starts_at_zero) {
  particle const p = make_particle({1.0, 1.0, 5.0, 5.0, 1.0, 1.0}, 0);
  EXPECT_DOUBLE_EQ(p.acceleration.x, 0.0);
  EXPECT_DOUBLE_EQ(p.acceleration.y, 0.0);
}

TEST(test_make_particle, id_is_the_one_given) {
  EXPECT_EQ(make_particle({1.0, 1.0, 5.0, 5.0, 0.0, 0.0}, 0).id, 0U);
  EXPECT_EQ(make_particle({1.0, 1.0, 5.0, 5.0, 0.0, 0.0}, 12'345).id, 12'345U);
}

TEST(test_make_particle, short_row_throws) {
  EXPECT_THROW((void) make_particle({1.0, 2.0, 3.0}, 0), std::out_of_range);
  EXPECT_THROW((void) make_particle({}, 0), std::out_of_range);
}

TEST(test_make_particle, extra_values_in_the_row_are_ignored) {
  particle const p = make_particle({2.0, 0.5, 3.0, 4.0, -1.0, 6.0, 99.0}, 1);
  EXPECT_DOUBLE_EQ(p.velocity.y, 6.0);
}

// particle

TEST(test_particle, a_default_particle_is_all_zeros) {
  particle const p;
  EXPECT_EQ(p.id, 0U);
  EXPECT_DOUBLE_EQ(p.mass, 0.0);
  EXPECT_DOUBLE_EQ(p.radius, 0.0);
  EXPECT_DOUBLE_EQ(p.position.x, 0.0);
  EXPECT_DOUBLE_EQ(p.acceleration.y, 0.0);
}

// remove_unused
// Collisions only mark the absorbed particle with UNUSED_ID; remove_unused
// is what actually erases it from the vector.

TEST(test_remove_unused, erases_only_the_particles_marked_unused) {
  std::vector<particle> ps{
      make_simple(0, 1.0),
      make_simple(constants::UNUSED_ID, 2.0),
      make_simple(2, 3.0),
      make_simple(constants::UNUSED_ID, 4.0),
  };

  remove_unused(ps);

  ASSERT_EQ(ps.size(), 2U);
  EXPECT_EQ(ps.at(0).id, 0U);
  EXPECT_EQ(ps.at(1).id, 2U);
}

TEST(test_remove_unused, keeps_the_relative_order_of_the_survivors) {
  std::vector<particle> ps{
      make_simple(5, 1.0),
      make_simple(constants::UNUSED_ID, 0.0),
      make_simple(3, 2.0),
      make_simple(9, 3.0),
  };

  remove_unused(ps);

  ASSERT_EQ(ps.size(), 3U);
  EXPECT_EQ(ps.at(0).id, 5U);
  EXPECT_EQ(ps.at(1).id, 3U);
  EXPECT_EQ(ps.at(2).id, 9U);
}

TEST(test_remove_unused, does_not_modify_the_survivors) {
  particle const p = make_particle({2.0, 0.5, 3.0, 4.0, -1.0, 6.0}, 7);
  std::vector<particle> ps{make_simple(constants::UNUSED_ID, 1.0), p};

  remove_unused(ps);

  ASSERT_EQ(ps.size(), 1U);
  EXPECT_EQ(ps.at(0).id, 7U);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 2.0);
  EXPECT_DOUBLE_EQ(ps.at(0).radius, 0.5);
  EXPECT_DOUBLE_EQ(ps.at(0).position.x, 3.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, 6.0);
}

TEST(test_remove_unused, nothing_marked_leaves_the_vector_unchanged) {
  std::vector<particle> ps{make_simple(0, 1.0), make_simple(1, 2.0)};

  remove_unused(ps);

  EXPECT_EQ(ps.size(), 2U);
}

TEST(test_remove_unused, everything_marked_empties_the_vector) {
  std::vector<particle> ps{
      make_simple(constants::UNUSED_ID, 1.0),
      make_simple(constants::UNUSED_ID, 2.0),
  };

  remove_unused(ps);

  EXPECT_TRUE(ps.empty());
}

TEST(test_remove_unused, empty_vector_is_fine) {
  std::vector<particle> ps;

  remove_unused(ps);

  EXPECT_TRUE(ps.empty());
}