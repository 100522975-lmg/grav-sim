
#include "simulation.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <vector>

#include "common.hpp"
#include "particle.hpp"
#include "point.hpp"

using common::point;
using constants::G;

namespace {

constexpr double tolerance = 1e-24;

// Creates a particle with radius 1 and zero acceleration.
particle make(std::size_t id, double mass, point pos, point vel) {
    return particle{id, mass, 1.0, pos, vel, point{}};
}

}  // namespace

// reset_acceleration

TEST(test_reset_acceleration, sets_every_acceleration_to_zero) {
    std::vector<particle> ps{
        make(0, 1.0, point{}, point{}),
        make(1, 1.0, point{}, point{})
    };

    ps.at(0).acceleration = point{3.0, 4.0};
    ps.at(1).acceleration = point{-1.0, 2.0};

    reset_acceleration(ps);

    EXPECT_DOUBLE_EQ(ps.at(0).acceleration.x, 0.0);
    EXPECT_DOUBLE_EQ(ps.at(0).acceleration.y, 0.0);
    EXPECT_DOUBLE_EQ(ps.at(1).acceleration.x, 0.0);
    EXPECT_DOUBLE_EQ(ps.at(1).acceleration.y, 0.0);
}

TEST(test_reset_acceleration, does_not_touch_the_other_fields) {
    particle p = make(4, 2.0, point{1.0, 2.0}, point{3.0, 4.0});
    p.radius = 1.5;

    std::vector<particle> ps{p};
    ps.at(0).acceleration = point{9.0, 9.0};

    reset_acceleration(ps);

    EXPECT_EQ(ps.at(0).id, 4U);
    EXPECT_DOUBLE_EQ(ps.at(0).mass, 2.0);
    EXPECT_DOUBLE_EQ(ps.at(0).radius, 1.5);
    EXPECT_DOUBLE_EQ(ps.at(0).position.x, 1.0);
    EXPECT_DOUBLE_EQ(ps.at(0).position.y, 2.0);
    EXPECT_DOUBLE_EQ(ps.at(0).velocity.x, 3.0);
    EXPECT_DOUBLE_EQ(ps.at(0).velocity.y, 4.0);
}

TEST(test_reset_acceleration, empty_vector_is_fine) {
    std::vector<particle> ps;

    reset_acceleration(ps);

    EXPECT_TRUE(ps.empty());
}

// accumulate_forces
// The acceleration field temporarily stores the accumulated force.

TEST(test_accumulate_forces, equal_and_opposite_on_both_particles) {
    // Distance = 5, with direction (0.6, 0.8).
    std::vector<particle> ps{
        make(0, 2.0, point{0.0, 0.0}, point{}),
        make(1, 5.0, point{3.0, 4.0}, point{})
    };

    accumulate_forces(ps);

    double const magnitude = G * 2.0 * 5.0 / 25.0;

    EXPECT_NEAR(ps.at(0).acceleration.x, magnitude * 0.6, tolerance);
    EXPECT_NEAR(ps.at(0).acceleration.y, magnitude * 0.8, tolerance);
    EXPECT_DOUBLE_EQ(
        ps.at(1).acceleration.x, -ps.at(0).acceleration.x);
    EXPECT_DOUBLE_EQ(
        ps.at(1).acceleration.y, -ps.at(0).acceleration.y);
}

TEST(test_accumulate_forces, adds_to_the_existing_values) {
    std::vector<particle> ps{
        make(0, 1.0, point{0.0, 0.0}, point{}),
        make(1, 1.0, point{1.0, 0.0}, point{})
    };

    accumulate_forces(ps);
    accumulate_forces(ps);

    EXPECT_NEAR(ps.at(0).acceleration.x, 2.0 * G, tolerance);
}

TEST(test_accumulate_forces, three_particles_add_both_forces_on_the_middle_one) {
    std::vector<particle> ps{
        make(0, 1.0, point{0.0, 0.0}, point{}),
        make(1, 1.0, point{1.0, 0.0}, point{}),
        make(2, 3.0, point{3.0, 0.0}, point{})
    };

    accumulate_forces(ps);

    double const from_first = -G;
    double const from_third = G * 1.0 * 3.0 / 4.0;

    EXPECT_NEAR(
        ps.at(1).acceleration.x, from_first + from_third, tolerance);
}

TEST(test_accumulate_forces, total_force_of_the_system_is_zero) {
    std::vector<particle> ps{
        make(0, 2.0, point{0.0, 0.0}, point{}),
        make(1, 3.0, point{4.0, 1.0}, point{}),
        make(2, 5.0, point{-2.0, 7.0}, point{})
    };

    accumulate_forces(ps);

    double total_x = 0.0;
    double total_y = 0.0;

    for (auto const& p : ps) {
        total_x += p.acceleration.x;
        total_y += p.acceleration.y;
    }

    EXPECT_NEAR(total_x, 0.0, tolerance);
    EXPECT_NEAR(total_y, 0.0, tolerance);
}

TEST(test_accumulate_forces, single_particle_and_empty_vector_are_fine) {
    std::vector<particle> one{
        make(0, 1.0, point{5.0, 5.0}, point{})
    };

    accumulate_forces(one);

    EXPECT_DOUBLE_EQ(one.at(0).acceleration.x, 0.0);
    EXPECT_DOUBLE_EQ(one.at(0).acceleration.y, 0.0);

    std::vector<particle> none;

    accumulate_forces(none);

    EXPECT_TRUE(none.empty());
}

// forces_to_acceleration

TEST(test_forces_to_acceleration, divides_each_force_by_its_own_mass) {
    std::vector<particle> ps{
        make(0, 2.0, point{}, point{}),
        make(1, 4.0, point{}, point{})
    };

    ps.at(0).acceleration = point{6.0, -8.0};
    ps.at(1).acceleration = point{6.0, -8.0};

    forces_to_acceleration(ps);

    EXPECT_DOUBLE_EQ(ps.at(0).acceleration.x, 3.0);
    EXPECT_DOUBLE_EQ(ps.at(0).acceleration.y, -4.0);
    EXPECT_DOUBLE_EQ(ps.at(1).acceleration.x, 1.5);
    EXPECT_DOUBLE_EQ(ps.at(1).acceleration.y, -2.0);
}

TEST(test_forces_to_acceleration, zero_force_gives_zero_acceleration) {
    std::vector<particle> ps{
        make(0, 7.0, point{}, point{})
    };

    forces_to_acceleration(ps);

    EXPECT_DOUBLE_EQ(ps.at(0).acceleration.x, 0.0);
    EXPECT_DOUBLE_EQ(ps.at(0).acceleration.y, 0.0);
}

// compute_acceleration

TEST(test_compute_acceleration, two_particles_on_the_x_axis) {
    std::vector<particle> ps{
        make(0, 2.0, point{0.0, 0.0}, point{}),
        make(1, 3.0, point{2.0, 0.0}, point{})
    };

    compute_acceleration(ps);

    EXPECT_NEAR(ps.at(0).acceleration.x, G * 3.0 / 4.0, tolerance);
    EXPECT_NEAR(ps.at(1).acceleration.x, -G * 2.0 / 4.0, tolerance);
    EXPECT_NEAR(ps.at(0).acceleration.y, 0.0, tolerance);
}

TEST(test_compute_acceleration, previous_values_are_discarded) {
    std::vector<particle> ps{
        make(0, 2.0, point{0.0, 0.0}, point{}),
        make(1, 3.0, point{2.0, 0.0}, point{})
    };

    ps.at(0).acceleration = point{100.0, 100.0};

    compute_acceleration(ps);
    compute_acceleration(ps);

    EXPECT_NEAR(ps.at(0).acceleration.x, G * 3.0 / 4.0, tolerance);
    EXPECT_NEAR(ps.at(0).acceleration.y, 0.0, tolerance);
}

TEST(test_compute_acceleration, single_particle_has_no_acceleration) {
    std::vector<particle> ps{
        make(0, 2.0, point{5.0, 5.0}, point{})
    };

    compute_acceleration(ps);

    EXPECT_DOUBLE_EQ(ps.at(0).acceleration.x, 0.0);
    EXPECT_DOUBLE_EQ(ps.at(0).acceleration.y, 0.0);
}

TEST(test_compute_acceleration, middle_particle_between_equal_masses_is_balanced) {
    std::vector<particle> ps{
        make(0, 4.0, point{0.0, 0.0}, point{}),
        make(1, 1.0, point{5.0, 0.0}, point{}),
        make(2, 4.0, point{10.0, 0.0}, point{})
    };

    compute_acceleration(ps);

    EXPECT_NEAR(ps.at(1).acceleration.x, 0.0, tolerance);
    EXPECT_GT(ps.at(0).acceleration.x, 0.0);
    EXPECT_LT(ps.at(2).acceleration.x, 0.0);
}

TEST(test_compute_acceleration, empty_vector_is_fine) {
    std::vector<particle> ps;

    compute_acceleration(ps);

    EXPECT_TRUE(ps.empty());
}

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

// update_position

TEST(test_update_position, adds_velocity_times_time_step) {
    particle p = make(
        0, 1.0, point{1.0, 1.0}, point{2.0, -4.0});

    update_position(p, 0.5);

    EXPECT_DOUBLE_EQ(p.position.x, 2.0);
    EXPECT_DOUBLE_EQ(p.position.y, -1.0);
}

TEST(test_update_position, zero_velocity_keeps_the_position) {
    particle p = make(
        0, 1.0, point{7.0, 8.0}, point{});

    update_position(p, 5.0);

    EXPECT_DOUBLE_EQ(p.position.x, 7.0);
    EXPECT_DOUBLE_EQ(p.position.y, 8.0);
}

// apply_rebound
// Enclosure: 10 x 10 unless otherwise stated; particle radius = 1.

TEST(test_apply_rebound, inside_the_enclosure_changes_nothing) {
    particle p = make(
        0, 1.0, point{5.0, 5.0}, point{-1.0, 2.0});

    apply_rebound(p, 10.0, 10.0);

    EXPECT_DOUBLE_EQ(p.position.x, 5.0);
    EXPECT_DOUBLE_EQ(p.position.y, 5.0);
    EXPECT_DOUBLE_EQ(p.velocity.x, -1.0);
    EXPECT_DOUBLE_EQ(p.velocity.y, 2.0);
}

TEST(test_apply_rebound, left_wall_flips_x_only) {
    particle p = make(
        0, 1.0, point{0.5, 5.0}, point{-2.0, 3.0});

    apply_rebound(p, 10.0, 10.0);

    EXPECT_DOUBLE_EQ(p.position.x, 1.0);
    EXPECT_DOUBLE_EQ(p.velocity.x, 2.0);
    EXPECT_DOUBLE_EQ(p.position.y, 5.0);
    EXPECT_DOUBLE_EQ(p.velocity.y, 3.0);
}

TEST(test_apply_rebound, right_wall_uses_the_width) {
    particle p = make(
        0, 1.0, point{9.5, 5.0}, point{2.0, 0.0});

    apply_rebound(p, 10.0, 20.0);

    EXPECT_DOUBLE_EQ(p.position.x, 9.0);
    EXPECT_DOUBLE_EQ(p.velocity.x, -2.0);
}

TEST(test_apply_rebound, top_wall_uses_the_height_not_the_width) {
    particle p = make(
        0, 1.0, point{5.0, 19.5}, point{1.0, 4.0});

    apply_rebound(p, 10.0, 20.0);

    EXPECT_DOUBLE_EQ(p.position.y, 19.0);
    EXPECT_DOUBLE_EQ(p.velocity.y, -4.0);
    EXPECT_DOUBLE_EQ(p.velocity.x, 1.0);
}

TEST(test_apply_rebound, bottom_wall_flips_y_only) {
    particle p = make(
        0, 1.0, point{5.0, 0.3}, point{1.0, -4.0});

    apply_rebound(p, 10.0, 10.0);

    EXPECT_DOUBLE_EQ(p.position.y, 1.0);
    EXPECT_DOUBLE_EQ(p.velocity.y, 4.0);
    EXPECT_DOUBLE_EQ(p.velocity.x, 1.0);
}

TEST(test_apply_rebound, corner_flips_both_axes) {
    particle p = make(
        0, 1.0, point{0.2, 9.9}, point{-1.0, 1.0});

    apply_rebound(p, 10.0, 10.0);

    EXPECT_DOUBLE_EQ(p.velocity.x, 1.0);
    EXPECT_DOUBLE_EQ(p.velocity.y, -1.0);
}

TEST(test_apply_rebound, exactly_touching_a_wall_counts_as_a_rebound) {
    particle p = make(
        0, 1.0, point{1.0, 5.0}, point{-2.0, 0.0});

    apply_rebound(p, 10.0, 10.0);

    EXPECT_DOUBLE_EQ(p.position.x, 1.0);
    EXPECT_DOUBLE_EQ(p.velocity.x, 2.0);
}

// step


TEST(test_step, isolated_particle_moves_in_a_straight_line) {
    std::vector<particle> ps;
    ps.push_back(make(0, 1.0, {5.0, 5.0}, {1.0, 2.0}));

    step(ps, 100.0, 100.0, 0.5);

    EXPECT_NEAR(ps.at(0).position.x, 5.5, 1e-9);
    EXPECT_NEAR(ps.at(0).position.y, 6.0, 1e-9);
}

TEST(test_step, position_uses_the_already_updated_velocity) {
    std::vector<particle> ps;
    ps.push_back(make(0, 1.0, {5.0, 5.0}, {1.0, 0.0}));
    ps.push_back(make(1, 1.0, {7.0, 5.0}, {0.0, 0.0}));

    step(ps, 100.0, 100.0, 0.1);

    EXPECT_NEAR(ps.at(0).position.x,
                5.0 + ps.at(0).velocity.x * 0.1, 1e-9);
    EXPECT_NEAR(ps.at(0).position.y,
                5.0 + ps.at(0).velocity.y * 0.1, 1e-9);
}

TEST(test_step, rebounds_on_the_wall_at_the_end_of_the_iteration) {
    std::vector<particle> ps;
    ps.push_back(make(0, 1.0, {9.0, 5.0}, {2.0, 0.0}));

    step(ps, 1.0, 10.0, 10.0);

    EXPECT_LE(ps.at(0).position.x, 10.0);
    EXPECT_LT(ps.at(0).velocity.x, 0.0);
}

TEST(test_step, all_forces_use_the_positions_of_the_start_of_the_iteration) {
    std::vector<particle> ps;
    ps.push_back(make(0, 1.0, {0.0, 0.0}, {1.0, 0.0}));
    ps.push_back(make(1, 1.0, {10.0, 0.0}, {-1.0, 0.0}));

    step(ps, 100.0, 100.0, 0.1);

    EXPECT_GT(ps.at(0).velocity.x, 1.0);
    EXPECT_LT(ps.at(1).velocity.x, -1.0);
}


TEST(test_step, gravity_matches_the_hand_computed_value) {
    std::vector<particle> ps;

    // Equal masses separated by 100 units.
    ps.push_back(make(0, 1.0e16, {100.0, 500.0}, {0.0, 0.0}));
    ps.push_back(make(1, 1.0e16, {200.0, 500.0}, {0.0, 0.0}));

    constexpr double width = 1000.0;
    constexpr double height = 1000.0;
    constexpr double dt = 1.0e-6;

    step(ps, width, height, dt);
    // Expected acceleration magnitude:
    // G * mass / distance^2 = 66.743 (approximately).
    EXPECT_NEAR(ps.at(0).velocity.x, 66.743 * dt, 1e-9);
    EXPECT_NEAR(ps.at(1).velocity.x, -66.743 * dt, 1e-9);

    EXPECT_NEAR(ps.at(0).velocity.y, 0.0, 1e-9);
    EXPECT_NEAR(ps.at(1).velocity.y, 0.0, 1e-9);
}

TEST(test_step, no_particles_is_fine) {
    std::vector<particle> ps;

    EXPECT_NO_THROW(step(ps, 100.0, 100.0, 0.1));
}
