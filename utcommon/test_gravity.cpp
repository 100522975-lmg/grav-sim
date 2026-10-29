#include "gravity.hpp"
#include <gtest/gtest.h>
#include <cmath>

using namespace common;

// Test the behavior of pair_force when both points have identical coordinates.
TEST(test_gravity, same_point) {
    point const p_i{5.0, 6.0};
    point const p_j{5.0, 6.0};

    point const force = pair_force(2.0, 2.0, p_i, p_j);

    EXPECT_FALSE(std::isfinite(force.x));
    EXPECT_FALSE(std::isfinite(force.y));
}

TEST(test_gravity, different_mass) {
    point const p_i{5.0, 6.0};
    point const p_j{5.0, 6.0};

    point const force = pair_force(2.0, 5.0, p_i, p_j);

    EXPECT_FALSE(std::isfinite(force.x));
    EXPECT_FALSE(std::isfinite(force.y));
}

//Test: x component = 0, y component not 0. 
TEST(test_gravity, x_0){
    point const pi = {0.0, 6.0}; 
    point const pj = {0.0, 1.0}; 

    point const force = pair_force(5.0, 1.0, pi, pj);

    EXPECT_DOUBLE_EQ(force.x, 0.0);
    EXPECT_NEAR(force.y, -constants::G / 5.0, 1e-20);
}

//Test: y component = 0, x component not 0.
TEST(test_gravity, y_0){
    point const pi = {2.0, 0.0}; 
    point const pj = {8.0, 0.0}; 

    point const force = pair_force(5.5, 3.2, pi, pj);

    EXPECT_NEAR(force.x, constants::G * 22.0 / 45.0, 1e-20);
    EXPECT_DOUBLE_EQ(force.y, 0.0);
}

//Test: two points separated with different masses. 
TEST(test_gravity, regular_test){
    point const pi = {9.0, 6.0}; 
    point const pj = {3.0, 3.0}; 

    point const force = pair_force(2.5, 10.0, pi, pj);

    double const factor = constants::G*(25/std::sqrt(std::pow(45,3)));

    EXPECT_NEAR(force.x, factor * -6.0, 1e-20);
    EXPECT_NEAR(force.y, factor* -3.0, 1e-20);
}
