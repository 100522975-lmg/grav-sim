
#include "soa.hpp"
#include <gtest/gtest.h>
#include <vector>
#include "collisions.hpp"


using namespace physics;

namespace{
    class SOACollisionsTest : public ::testing::Test {
    protected:
        static ParticleSOA make_particles(
            const std::vector<std::vector<double>>& data
        ) {
            return ParticleSOA(data);
        }
    };
}
// Tests find_collision()

TEST_F(SOACollisionsTest, find_collisions_empty_particles) {
    auto particles = make_particles({});
    EXPECT_TRUE(physics::find_collisions(particles).empty());
}

TEST_F(SOACollisionsTest, find_collisions_single_particle) {
    auto particles = make_particles({
        {1.0, 1.0, 0.0, 0.0, 0.0, 0.0}
    });
    EXPECT_TRUE(physics::find_collisions(particles).empty());
}

TEST_F(SOACollisionsTest, find_collisions_no_collisions) {
    auto particles = make_particles({
        {1.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {1.0, 1.0, 5.0, 0.0, 0.0, 0.0},
        {1.0, 1.0, 10.0, 0.0, 0.0, 0.0}
    });
    EXPECT_TRUE(physics::find_collisions(particles).empty());
}

TEST_F(SOACollisionsTest, find_collisions_overlapping_particles) {
    auto particles = make_particles({
        {2.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {3.0, 1.0, 1.0, 0.0, 0.0, 0.0}
    });

    const auto collisions = physics::find_collisions(particles);

    ASSERT_EQ(collisions.size(), 1);
    EXPECT_EQ(collisions.at(0).i, 0);
    EXPECT_EQ(collisions.at(0).j, 1);
}

TEST_F(SOACollisionsTest, find_collisions_returns_all_overlapping_pairs) {
    auto particles = make_particles({
        {1.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {1.0, 1.0, 1.0, 0.0, 0.0, 0.0},
        {1.0, 1.0, 20.0, 0.0, 0.0, 0.0}
    });

    const auto collisions = physics::find_collisions(particles);

    ASSERT_EQ(collisions.size(), 1);
    EXPECT_EQ(collisions.at(0).i, 0);
    EXPECT_EQ(collisions.at(0).j, 1);
}


// Tests resolve_collisions())

TEST_F(SOACollisionsTest, resolve_collisions_marks_absorbed_particle) {
    auto particles = make_particles({
        {5.0, 1.0, 0.0, 0.0, 1.0, 0.0},
        {2.0, 1.0, 1.0, 0.0, 0.0, 1.0}
    });

    physics::resolve_collisions(particles);

    ASSERT_EQ(particles.id.size(), 2);
    EXPECT_EQ(particles.id.at(0), 0);
    EXPECT_EQ(particles.id.at(1), constants::UNUSED_ID);
    EXPECT_DOUBLE_EQ(particles.mass.at(0), 7.0);

    // The absorber keeps its original position.
    EXPECT_DOUBLE_EQ(particles.position.at(0).x, 0.0);
    EXPECT_DOUBLE_EQ(particles.position.at(0).y, 0.0);
}

