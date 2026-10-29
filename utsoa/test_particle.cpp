
#include "soa.hpp"
#include <gtest/gtest.h>
#include <vector>

TEST(ParticleSOATest, empty_input_constructs_empty_soa) {
    const ParticleSOA soa({});
    EXPECT_TRUE(soa.id.empty());
    EXPECT_TRUE(soa.mass.empty());
    EXPECT_TRUE(soa.radius.empty());
    EXPECT_TRUE(soa.position.empty());
    EXPECT_TRUE(soa.velocity.empty());
}

TEST(ParticleSOATest, single_particle_correctly_populated) {
    const std::vector<std::vector<double>> input = {
        {10.5, 0.5, 1.0, 2.0, -1.0, -2.0}
    };

    const ParticleSOA soa(input);

    ASSERT_EQ(soa.id.size(), 1U);
    EXPECT_EQ(soa.id.at(0), 0U);
    EXPECT_DOUBLE_EQ(soa.mass.at(0), 10.5);
    EXPECT_DOUBLE_EQ(soa.radius.at(0), 0.5);
    EXPECT_DOUBLE_EQ(soa.position.at(0).x, 1.0);
    EXPECT_DOUBLE_EQ(soa.position.at(0).y, 2.0);
    EXPECT_DOUBLE_EQ(soa.velocity.at(0).x, -1.0);
    EXPECT_DOUBLE_EQ(soa.velocity.at(0).y, -2.0);
}

TEST(ParticleSOATest, multiple_particles_correctly_populated) {
    const std::vector<std::vector<double>> input = {
        {1.0, 0.1, 0.0, 0.0, 1.0, 0.0},
        {2.5, 0.2, 10.0, 5.0, -0.5, 1.5},
        {5.0, 0.8, -3.0, 4.0, 0.0, 0.0}
    };

    const ParticleSOA soa(input);

    ASSERT_EQ(soa.id.size(), 3U);
    EXPECT_EQ(soa.id.at(0), 0U);
    EXPECT_EQ(soa.id.at(1), 1U);
    EXPECT_EQ(soa.id.at(2), 2U);

    EXPECT_DOUBLE_EQ(soa.mass.at(1), 2.5);
    EXPECT_DOUBLE_EQ(soa.radius.at(2), 0.8);
    EXPECT_DOUBLE_EQ(soa.position.at(1).x, 10.0);
    EXPECT_DOUBLE_EQ(soa.position.at(1).y, 5.0);
    EXPECT_DOUBLE_EQ(soa.velocity.at(1).x, -0.5);
    EXPECT_DOUBLE_EQ(soa.velocity.at(1).y, 1.5);
}

TEST(ParticleSOATest, handles_zero_and_negative_values) {
    const std::vector<std::vector<double>> input = {
        {0.0, 0.0, -100.5, -200.5, 0.0, -1.0}
    };

    const ParticleSOA soa(input);

    ASSERT_EQ(soa.mass.size(), 1U);
    EXPECT_DOUBLE_EQ(soa.mass.at(0), 0.0);
    EXPECT_DOUBLE_EQ(soa.radius.at(0), 0.0);
    EXPECT_DOUBLE_EQ(soa.position.at(0).x, -100.5);
    EXPECT_DOUBLE_EQ(soa.position.at(0).y, -200.5);
    EXPECT_DOUBLE_EQ(soa.velocity.at(0).x, 0.0);
    EXPECT_DOUBLE_EQ(soa.velocity.at(0).y, -1.0);
}

TEST(ParticleSOATest, handles_large_floating_point_values) {
    const double huge = 1e308;
    const double tiny = 1e-308;

    const std::vector<std::vector<double>> input = {
        {huge, tiny, huge, 0.0, tiny, 0.0}
    };

    const ParticleSOA soa(input);

    ASSERT_EQ(soa.mass.size(), 1U);
    EXPECT_DOUBLE_EQ(soa.mass.at(0), huge);
    EXPECT_DOUBLE_EQ(soa.radius.at(0), tiny);
    EXPECT_DOUBLE_EQ(soa.position.at(0).x, huge);
    EXPECT_DOUBLE_EQ(soa.velocity.at(0).x, tiny);
}

TEST(ParticleSOATest, empty_rows_preserve_original_row_indices) {
    const std::vector<std::vector<double>> input = {
        {1.0, 1.0, 10.0, 10.0, 0.0, 0.0},
        {},
        {2.0, 1.0, 20.0, 20.0, 0.0, 0.0}
    };

    const ParticleSOA soa(input);

    ASSERT_EQ(soa.id.size(), 2U);
    EXPECT_EQ(soa.id.at(0), 0U);
    EXPECT_EQ(soa.id.at(1), 2U);
}
