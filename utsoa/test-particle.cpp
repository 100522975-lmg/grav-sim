#include "soa.hpp"

#include <gtest/gtest.h>
#include <stdexcept>
#include <gtest/gtest.h>
#include "soa.hpp"
#include <vector>

class ParticleSOATest : public ::testing::Test {
protected:
    std::vector<double> make_particle_data(int id, double mass, double radius,
                                           double px, double py,
                                           double vx, double vy) {
        return {static_cast<double>(id), mass, radius, px, py, vx, vy};
    }
};

TEST_F(ParticleSOATest, empty_input_constructs_empty_soa) {
    std::vector<std::vector<double>> empty_particles;
    ParticleSOA soa(empty_particles);

    EXPECT_TRUE(soa.id.empty());
    EXPECT_TRUE(soa.mass.empty());
    EXPECT_TRUE(soa.radius.empty());
    EXPECT_TRUE(soa.position.empty());
    EXPECT_TRUE(soa.velocity.empty());
}

TEST_F(ParticleSOATest, single_particle_correctly_populated) {
    std::vector<std::vector<double>> input = {
        make_particle_data(1, 10.5, 0.5, 1.0, 2.0, -1.0, -2.0)
    };

    ParticleSOA soa(input);

    ASSERT_EQ(soa.id.size(), 1u);
    ASSERT_EQ(soa.mass.size(), 1u);
    ASSERT_EQ(soa.radius.size(), 1u);
    ASSERT_EQ(soa.position.size(), 1u);
    ASSERT_EQ(soa.velocity.size(), 1u);

    EXPECT_EQ(soa.id[0], 1);
    EXPECT_DOUBLE_EQ(soa.mass[0], 10.5);
    EXPECT_DOUBLE_EQ(soa.radius[0], 0.5);

    EXPECT_DOUBLE_EQ(soa.position[0].x, 1.0);
    EXPECT_DOUBLE_EQ(soa.position[0].y, 2.0);
    EXPECT_DOUBLE_EQ(soa.velocity[0].x, -1.0);
    EXPECT_DOUBLE_EQ(soa.velocity[0].y, -2.0);
}

TEST_F(ParticleSOATest, multiple_particles_correctly_populated) {
    std::vector<std::vector<double>> input = {
        make_particle_data(101, 1.0, 0.1, 0.0, 0.0, 1.0, 0.0),
        make_particle_data(102, 2.5, 0.2, 10.0, 5.0, -0.5, 1.5),
        make_particle_data(103, 5.0, 0.8, -3.0, 4.0, 0.0, 0.0)
    };

    ParticleSOA soa(input);

    size_t expected_count = input.size();
    EXPECT_EQ(soa.id.size(), expected_count);
    EXPECT_EQ(soa.mass.size(), expected_count);
    EXPECT_EQ(soa.radius.size(), expected_count);
    EXPECT_EQ(soa.position.size(), expected_count);
    EXPECT_EQ(soa.velocity.size(), expected_count);

    EXPECT_EQ(soa.id[0], 101);
    EXPECT_EQ(soa.id[1], 102);
    EXPECT_EQ(soa.id[2], 103);

    EXPECT_DOUBLE_EQ(soa.mass[1], 2.5);
    EXPECT_DOUBLE_EQ(soa.radius[2], 0.8);

    EXPECT_DOUBLE_EQ(soa.position[1].x, 10.0);
    EXPECT_DOUBLE_EQ(soa.position[1].y, 5.0);
    EXPECT_DOUBLE_EQ(soa.velocity[1].x, -0.5);
    EXPECT_DOUBLE_EQ(soa.velocity[1].y, 1.5);
}

TEST_F(ParticleSOATest, handles_zero_and_negative_values) {
    std::vector<std::vector<double>> input = {
        make_particle_data(-5, 0.0, 0.0, -100.5, -200.5, 0.0, -1.0)
    };

    ParticleSOA soa(input);

    EXPECT_EQ(soa.id[0], -5);
    EXPECT_DOUBLE_EQ(soa.mass[0], 0.0);
    EXPECT_DOUBLE_EQ(soa.radius[0], 0.0);
    EXPECT_DOUBLE_EQ(soa.position[0].x, -100.5);
    EXPECT_DOUBLE_EQ(soa.position[0].y, -200.5);
    EXPECT_DOUBLE_EQ(soa.velocity[0].x, 0.0);
    EXPECT_DOUBLE_EQ(soa.velocity[0].y, -1.0);
}

TEST_F(ParticleSOATest, handles_large_floating_point_values) {
    double huge_val = 1e308;
    double tiny_val = 1e-308;

    std::vector<std::vector<double>> input = {
        make_particle_data(999, huge_val, tiny_val, huge_val, 0.0, tiny_val, 0.0)
    };

    ParticleSOA soa(input);

    EXPECT_DOUBLE_EQ(soa.mass[0], huge_val);
    EXPECT_DOUBLE_EQ(soa.radius[0], tiny_val);
    EXPECT_DOUBLE_EQ(soa.position[0].x, huge_val);
    EXPECT_DOUBLE_EQ(soa.velocity[0].x, tiny_val);
}

TEST_F(ParticleSOATest, large_number_of_particles_stress_test) {
    const size_t num_particles = 100000;
    std::vector<std::vector<double>> input;
    input.reserve(num_particles);

    for (size_t i = 0; i < num_particles; ++i) {
        int id = static_cast<int>(i);
        double d_i = static_cast<double>(i);
        input.push_back(make_particle_data(id, d_i * 0.1, 1.0, d_i, d_i, 0.1, 0.2));
    }

    ParticleSOA soa(input);

    ASSERT_EQ(soa.id.size(), num_particles);
    ASSERT_EQ(soa.mass.size(), num_particles);
    ASSERT_EQ(soa.radius.size(), num_particles);
    ASSERT_EQ(soa.position.size(), num_particles);
    ASSERT_EQ(soa.velocity.size(), num_particles);

    EXPECT_EQ(soa.id[0], 0);
    EXPECT_EQ(soa.id[num_particles / 2], static_cast<int>(num_particles / 2));
    EXPECT_EQ(soa.id[num_particles - 1], static_cast<int>(num_particles - 1));
}
