#include "collisions.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <span>
#include <vector>

#include "common.hpp"
#include "particle.hpp"
#include "point.hpp"

using common::point;

namespace {

struct particle_motion {
  point position;
  point velocity;
};

particle make_p(std::size_t id, double mass, double radius, particle_motion motion) {
  return particle{id, mass, radius, motion.position, motion.velocity, point{0.0, 0.0}};
}

}  // namespace

// find_collisions
// Returns every colliding pair (as indices into the span) in lexicographic order.

TEST(test_find_collisions, returns_empty_when_no_particles_collide) {
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{10.0, 0.0}, point{}}),
      make_p(2, 1.0, 1.0, {point{0.0, 10.0}, point{}}),
  };

  EXPECT_TRUE(aos::find_collisions(ps).empty());
}

TEST(test_find_collisions, returns_empty_for_empty_or_single_particle) {
  std::vector<particle> const empty_ps;
  EXPECT_TRUE(aos::find_collisions(empty_ps).empty());

  std::vector<particle> const single_ps{make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}})};
  EXPECT_TRUE(aos::find_collisions(single_ps).empty());
}

TEST(test_find_collisions, returns_a_single_overlapping_pair) {
  std::vector<particle> const ps{
      make_p(0, 2.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 3.0, 1.0, {point{1.0, 0.0}, point{}}),
  };

  auto const result = aos::find_collisions(ps);

  ASSERT_EQ(result.size(), 1U);
  EXPECT_EQ(result.at(0).i, 0U);
  EXPECT_EQ(result.at(0).j, 1U);
}

TEST(test_find_collisions, returns_every_colliding_pair_in_lexicographic_order) {
  // 0-1 are 2.0 apart (touching, not colliding); 0-2 and 1-2 do collide.
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{0.0, 2.0}, point{}}),
      make_p(2, 1.0, 1.0, {point{0.0, 0.05}, point{}}),
  };

  auto const result = aos::find_collisions(ps);

  ASSERT_EQ(result.size(), 2U);
  EXPECT_EQ(result.at(0).i, 0U);
  EXPECT_EQ(result.at(0).j, 2U);
  EXPECT_EQ(result.at(1).i, 1U);
  EXPECT_EQ(result.at(1).j, 2U);
}

TEST(test_find_collisions, all_pairs_of_a_cluster_are_reported_in_order) {
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{0.1, 0.0}, point{}}),
      make_p(2, 1.0, 1.0, {point{0.2, 0.0}, point{}}),
  };

  auto const result = aos::find_collisions(ps);

  ASSERT_EQ(result.size(), 3U);
  EXPECT_EQ(result.at(0).i, 0U);
  EXPECT_EQ(result.at(0).j, 1U);
  EXPECT_EQ(result.at(1).i, 0U);
  EXPECT_EQ(result.at(1).j, 2U);
  EXPECT_EQ(result.at(2).i, 1U);
  EXPECT_EQ(result.at(2).j, 2U);
}

TEST(test_find_collisions, touching_at_exact_radius_distance_is_not_a_collision) {
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{2.0, 0.0}, point{}}),
  };

  EXPECT_TRUE(aos::find_collisions(ps).empty());
}

TEST(test_find_collisions, uses_the_sum_of_both_radii) {
  // Distance 3.0; radii 1.0 + 2.5 = 3.5 -> colliding.
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 2.5, {point{3.0, 0.0}, point{}}),
  };

  EXPECT_EQ(aos::find_collisions(ps).size(), 1U);
}

TEST(test_find_collisions, reports_indices_not_particle_ids) {
  // Ids are deliberately unrelated to the positions in the vector.
  std::vector<particle> const ps{
      make_p(40, 1.0, 1.0, {point{50.0, 50.0}, point{}}),
      make_p(17, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(3, 1.0, 1.0, {point{0.5, 0.0}, point{}}),
  };

  auto const result = aos::find_collisions(ps);

  ASSERT_EQ(result.size(), 1U);
  EXPECT_EQ(result.at(0).i, 1U);
  EXPECT_EQ(result.at(0).j, 2U);
}

TEST(test_find_collisions, works_on_a_sub_span) {
  std::vector<particle> const ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 1.0, 1.0, {point{0.5, 0.0}, point{}}),
      make_p(2, 1.0, 1.0, {point{100.0, 0.0}, point{}}),
  };

  // Skipping the first particle removes the only collision.
  auto const result = aos::find_collisions(std::span<const particle>{ps}.subspan(1));

  EXPECT_TRUE(result.empty());
}

// resolve_collisions
// Absorbed particles are NOT erased: they are only marked with UNUSED_ID.
// Callers must run remove_unused() afterwards.

TEST(test_resolve_collisions, higher_mass_absorbs_lower_mass) {
  std::vector<particle> ps{
      make_p(0, 10.0, 1.0, {point{0.0, 0.0}, point{1.0, 0.0}}),
      make_p(1, 2.0, 1.0, {point{0.05, 0.0}, point{0.0, 2.0}}),
  };

  aos::resolve_collisions(ps);

  EXPECT_EQ(ps.at(0).id, 0U);
  EXPECT_EQ(ps.at(1).id, constants::UNUSED_ID);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 12.0);
  EXPECT_DOUBLE_EQ(ps.at(0).position.x, 0.0);
  EXPECT_DOUBLE_EQ(ps.at(0).position.y, 0.0);
}

TEST(test_resolve_collisions, absorber_velocity_is_the_sum_of_both_velocities) {
  std::vector<particle> ps{
      make_p(0, 10.0, 1.0, {point{0.0, 0.0}, point{1.0, 0.5}}),
      make_p(1, 2.0, 1.0, {point{0.05, 0.0}, point{-3.0, 2.0}}),
  };

  aos::resolve_collisions(ps);

  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, -2.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, 2.5);
}

TEST(test_resolve_collisions, absorber_keeps_its_radius_and_acceleration) {
  std::vector<particle> ps{
      make_p(0, 10.0, 1.5, {point{0.0, 0.0}, point{}}),
      make_p(1, 2.0, 1.0, {point{0.05, 0.0}, point{}}),
  };
  ps.at(0).acceleration = point{4.0, 5.0};

  aos::resolve_collisions(ps);

  EXPECT_DOUBLE_EQ(ps.at(0).radius, 1.5);
  EXPECT_DOUBLE_EQ(ps.at(0).acceleration.x, 4.0);
  EXPECT_DOUBLE_EQ(ps.at(0).acceleration.y, 5.0);
}

TEST(test_resolve_collisions, later_particle_absorbs_when_it_is_heavier) {
  // Same as above but the heavy particle comes second: the p_j branch.
  std::vector<particle> ps{
      make_p(0, 2.0, 1.0, {point{0.0, 0.0}, point{0.0, 2.0}}),
      make_p(1, 10.0, 1.0, {point{0.05, 0.0}, point{1.0, 0.0}}),
  };

  aos::resolve_collisions(ps);

  EXPECT_EQ(ps.at(0).id, constants::UNUSED_ID);
  EXPECT_EQ(ps.at(1).id, 1U);
  EXPECT_DOUBLE_EQ(ps.at(1).mass, 12.0);
  EXPECT_DOUBLE_EQ(ps.at(1).velocity.x, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(1).velocity.y, 2.0);
  EXPECT_DOUBLE_EQ(ps.at(1).position.x, 0.05);
}

TEST(test_resolve_collisions, equal_mass_larger_radius_absorbs) {
  std::vector<particle> ps{
      make_p(0, 5.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 5.0, 2.0, {point{0.5, 0.0}, point{}}),
  };

  aos::resolve_collisions(ps);

  EXPECT_EQ(ps.at(0).id, constants::UNUSED_ID);
  EXPECT_EQ(ps.at(1).id, 1U);
  EXPECT_DOUBLE_EQ(ps.at(1).mass, 10.0);
}

TEST(test_resolve_collisions, equal_mass_and_radius_lowest_id_absorbs) {
  // The lower id sits in the second slot, so it must still win.
  std::vector<particle> ps{
      make_p(8, 5.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(2, 5.0, 1.0, {point{0.5, 0.0}, point{}}),
  };

  aos::resolve_collisions(ps);

  EXPECT_EQ(ps.at(0).id, constants::UNUSED_ID);
  EXPECT_EQ(ps.at(1).id, 2U);
  EXPECT_DOUBLE_EQ(ps.at(1).mass, 10.0);
}

TEST(test_resolve_collisions, resolves_multiple_cascading_collisions) {
  // All three overlap each other; the heaviest one absorbs the rest.
  std::vector<particle> ps{
      make_p(0, 10.0, 1.0, {point{0.0, 0.0}, point{1.0, 0.0}}),
      make_p(1, 2.0, 1.0, {point{0.05, 0.0}, point{0.0, 1.0}}),
      make_p(2, 3.0, 1.0, {point{0.08, 0.0}, point{0.0, 0.0}}),
  };

  aos::resolve_collisions(ps);
  remove_unused(ps);

  ASSERT_EQ(ps.size(), 1U);
  EXPECT_EQ(ps.at(0).id, 0U);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 15.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, 1.0);
}

TEST(test_resolve_collisions, absorbed_particle_does_not_take_part_in_later_collisions) {
  // 0-1 and 1-2 collide, 0-2 do not. Pair (0,1) is resolved first, so
  // particle 1 is already gone when pair (1,2) comes up and must be skipped.
  std::vector<particle> ps{
      make_p(0, 5.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 3.0, 1.0, {point{1.5, 0.0}, point{}}),
      make_p(2, 10.0, 1.0, {point{3.0, 0.0}, point{}}),
  };

  aos::resolve_collisions(ps);

  EXPECT_EQ(ps.at(0).id, 0U);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 8.0);
  EXPECT_EQ(ps.at(1).id, constants::UNUSED_ID);
  EXPECT_EQ(ps.at(2).id, 2U);
  EXPECT_DOUBLE_EQ(ps.at(2).mass, 10.0);
}

TEST(test_resolve_collisions, particles_already_marked_unused_are_skipped) {
  std::vector<particle> ps{
      make_p(constants::UNUSED_ID, 100.0, 1.0, {point{0.0, 0.0}, point{9.0, 9.0}}),
      make_p(1, 2.0, 1.0, {point{0.05, 0.0}, point{1.0, 1.0}}),
  };

  aos::resolve_collisions(ps);

  EXPECT_EQ(ps.at(0).id, constants::UNUSED_ID);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 100.0);
  EXPECT_EQ(ps.at(1).id, 1U);
  EXPECT_DOUBLE_EQ(ps.at(1).mass, 2.0);
  EXPECT_DOUBLE_EQ(ps.at(1).velocity.x, 1.0);
}

TEST(test_resolve_collisions, does_not_erase_particles_from_the_vector) {
  std::vector<particle> ps{
      make_p(0, 10.0, 1.0, {point{0.0, 0.0}, point{}}),
      make_p(1, 2.0, 1.0, {point{0.05, 0.0}, point{}}),
  };

  aos::resolve_collisions(ps);
  EXPECT_EQ(ps.size(), 2U);

  remove_unused(ps);
  EXPECT_EQ(ps.size(), 1U);
}

TEST(test_resolve_collisions, no_op_when_no_collisions) {
  std::vector<particle> ps{
      make_p(0, 1.0, 1.0, {point{0.0, 0.0}, point{1.0, 2.0}}),
      make_p(1, 3.0, 1.0, {point{10.0, 10.0}, point{3.0, 4.0}}),
  };

  aos::resolve_collisions(ps);

  ASSERT_EQ(ps.size(), 2U);
  EXPECT_EQ(ps.at(0).id, 0U);
  EXPECT_EQ(ps.at(1).id, 1U);
  EXPECT_DOUBLE_EQ(ps.at(0).mass, 1.0);
  EXPECT_DOUBLE_EQ(ps.at(1).mass, 3.0);
  EXPECT_DOUBLE_EQ(ps.at(1).velocity.y, 4.0);
}

TEST(test_resolve_collisions, empty_vector_handled_gracefully) {
  std::vector<particle> ps;
  EXPECT_NO_THROW(aos::resolve_collisions(ps));
  EXPECT_TRUE(ps.empty());
}