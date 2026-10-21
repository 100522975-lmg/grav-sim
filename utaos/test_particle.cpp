#include "particle.hpp"

#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

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

TEST(test_particle, a_default_particle_is_all_zeros) {
  particle const p;
  EXPECT_EQ(p.id, 0U);
  EXPECT_DOUBLE_EQ(p.mass, 0.0);
  EXPECT_DOUBLE_EQ(p.radius, 0.0);
  EXPECT_DOUBLE_EQ(p.position.x, 0.0);
  EXPECT_DOUBLE_EQ(p.acceleration.y, 0.0);
}