
#include "soa.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>
#include "collisions.hpp"


using namespace physics;

class SOACollisionsTest : public ::testing::Test {
protected:
    ParticleSOA make_particles(
        const std::vector<std::vector<double>>& data
    ) {
        return ParticleSOA(data);
    }
};

// Tests find_collision()

TEST_F(SOACollisionsTest, find_collision_empty_particles) {
    auto particles = make_particles({});

    EXPECT_TRUE(physics::find_collision(particles).empty());
}

TEST_F(SOACollisionsTest, find_collision_single_particle) {
    auto particles = make_particles({
        {1.0, 1.0, 0.0, 0.0, 0.0, 0.0}
    });

    EXPECT_TRUE(physics::find_collision(particles).empty());
}

TEST_F(SOACollisionsTest, find_collision_no_collisions) {
    auto particles = make_particles({
        {1.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {1.0, 1.0, 5.0, 0.0, 0.0, 0.0},
        {1.0, 1.0, 10.0, 0.0, 0.0, 0.0}
    });

    EXPECT_TRUE(physics::find_collision(particles).empty());
}

TEST_F(SOACollisionsTest, find_collision_overlapping_particles) {
    auto particles = make_particles({
        {2.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {3.0, 1.0, 1.0, 0.0, 0.0, 0.0}
    });

    const auto collision = physics::find_collision(particles);

    ASSERT_EQ(collision.size(), 2);
    EXPECT_EQ(collision[0], 0);
    EXPECT_EQ(collision[1], 1);
}

TEST_F(SOACollisionsTest, find_collision_returns_first_overlapping_pair) {
    auto particles = make_particles({
        {1.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {1.0, 1.0, 1.0, 0.0, 0.0, 0.0},
        {1.0, 1.0, 20.0, 0.0, 0.0, 0.0}
    });

    const auto collision = physics::find_collision(particles);

    ASSERT_EQ(collision.size(), 2);
    EXPECT_EQ(collision[0], 0);
    EXPECT_EQ(collision[1], 1);
}


// Tests merge_particle()

TEST_F(SOACollisionsTest, merge_particle_adds_mass_and_removes_absorbed) {
    auto particles = make_particles({
        {2.0, 1.0, 0.0, 0.0, 1.0, 2.0},
        {3.0, 1.0, 0.5, 0.0, 4.0, 5.0}
    });

    physics::merge_particle(particles, 0, 1);

    ASSERT_EQ(particles.id.size(), 1);
    EXPECT_DOUBLE_EQ(particles.mass[0], 5.0);
}

TEST_F(SOACollisionsTest, merge_particle_preserves_absorber_position) {
    auto particles = make_particles({
        {2.0, 1.0, 3.0, 4.0, 1.0, 2.0},
        {3.0, 1.0, 3.5, 4.0, 4.0, 5.0}
    });

    physics::merge_particle(particles, 0, 1);

    ASSERT_EQ(particles.position.size(), 1);
    EXPECT_DOUBLE_EQ(particles.position[0].x, 3.0);
    EXPECT_DOUBLE_EQ(particles.position[0].y, 4.0);
}

TEST_F(SOACollisionsTest, merge_particle_removes_absorbed_particle_when_not_last) {
    auto particles = make_particles({
        {2.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {3.0, 1.0, 0.5, 0.0, 0.0, 0.0},
        {4.0, 1.0, 10.0, 0.0, 0.0, 0.0}
    });

    physics::merge_particle(particles, 0, 1);

    ASSERT_EQ(particles.id.size(), 2);
    ASSERT_EQ(particles.mass.size(), 2);
    ASSERT_EQ(particles.radius.size(), 2);
    ASSERT_EQ(particles.position.size(), 2);
    ASSERT_EQ(particles.velocity.size(), 2);

    EXPECT_DOUBLE_EQ(particles.mass[0], 5.0);
    // The implementation replaces the removed slot with the last particle.
    EXPECT_DOUBLE_EQ(particles.mass[1], 4.0);
    EXPECT_EQ(particles.id[1], 2);
}

TEST_F(SOACollisionsTest, merge_particle_handles_absorbed_particle_at_last_index) {
    auto particles = make_particles({
        {2.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {3.0, 1.0, 0.5, 0.0, 0.0, 0.0}
    });

    physics::merge_particle(particles, 0, 1);

    ASSERT_EQ(particles.id.size(), 1);
    EXPECT_DOUBLE_EQ(particles.mass[0], 5.0);
    EXPECT_EQ(particles.id[0], 0);
}

// Tests resolve_collisions()

TEST_F(SOACollisionsTest, resolve_collisions_does_nothing_without_collisions) {
    auto particles = make_particles({
        {2.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {3.0, 1.0, 10.0, 0.0, 0.0, 0.0}
    });

    physics::resolve_collisions(particles);

    ASSERT_EQ(particles.id.size(), 2);
    EXPECT_DOUBLE_EQ(particles.mass[0], 2.0);
    EXPECT_DOUBLE_EQ(particles.mass[1], 3.0);
}

TEST_F(SOACollisionsTest, resolve_collisions_heavier_particle_absorbs_lighter) {
    auto particles = make_particles({
        {5.0, 1.0, 0.0, 0.0, 1.0, 0.0},
        {2.0, 1.0, 1.0, 0.0, 0.0, 1.0}
    });

    physics::resolve_collisions(particles);

    ASSERT_EQ(particles.id.size(), 1);
    EXPECT_EQ(particles.id[0], 0);
    EXPECT_DOUBLE_EQ(particles.mass[0], 7.0);
}

TEST_F(SOACollisionsTest, resolve_collisions_repeats_until_no_collisions_remain) {
    auto particles = make_particles({
        {5.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {2.0, 1.0, 1.0, 0.0, 0.0, 0.0},
        {1.0, 1.0, 2.0, 0.0, 0.0, 0.0}
    });

    physics::resolve_collisions(particles);

    ASSERT_EQ(particles.id.size(), 1);
    EXPECT_DOUBLE_EQ(particles.mass[0], 8.0);
}
