#include "gravity.hpp"
#include "rebound.hpp"
#include "point.hpp"
#include <gtest/gtest.h>
#include <cmath>

using namespace common;

// The ball is outside the left bound.
TEST (test_rebound, left_border) {
    point p = {2.0, 30.0}; // x = 2, radius 5, it will lay outside the left wall. 
    point velocity = {-3.0, 1.0};

    rebound_effect(p, velocity, 5.0, 100.0, 80.0);

    EXPECT_DOUBLE_EQ(p.x, 5.0);
    EXPECT_DOUBLE_EQ(velocity.x, 3.0); 
    EXPECT_DOUBLE_EQ(p.y, 30.0);
    EXPECT_DOUBLE_EQ(velocity.y, 1.0);
}

// The ball is outside the right bound.
TEST (test_rebound, right_border) {
    point p = {98.0, 30.0};
    point velocity = {3.0, 1.0};

    rebound_effect(p, velocity, 5.0, 100.0, 80.0);

    EXPECT_DOUBLE_EQ(p.x, 95.0);
    EXPECT_DOUBLE_EQ(velocity.x, -3.0);
    EXPECT_DOUBLE_EQ(p.y, 30.0);
    EXPECT_DOUBLE_EQ(velocity.y, 1.0);
}

// The ball is outside the upper bound.
TEST (test_rebound, upper_border) {
    point p = {40.0, 2.0};
    point velocity = {1.0, -4.0};

    rebound_effect(p, velocity, 5.0, 100.0, 80.0);

    EXPECT_DOUBLE_EQ(p.y, 5.0);
    EXPECT_DOUBLE_EQ(velocity.y, 4.0);
    EXPECT_DOUBLE_EQ(p.x, 40.0);
    EXPECT_DOUBLE_EQ(velocity.x, 1.0);
}

// The ball is outside the lower bound.
TEST (test_rebound, lower_border) {
    point p = {40.0, 78.0};
    point velocity = {1.0, 4.0};

    rebound_effect(p, velocity, 5.0, 100.0, 80.0);

    EXPECT_DOUBLE_EQ(p.y, 75.0);
    EXPECT_DOUBLE_EQ(velocity.y, -4.0);
    EXPECT_DOUBLE_EQ(p.x, 40.0);
    EXPECT_DOUBLE_EQ(velocity.x, 1.0);
}

// The ball is outside the upper left corner.
TEST (test_rebound, corner) {
    point p = {2.0, 2.0};
    point velocity = {-3.0, -4.0};

    rebound_effect(p, velocity, 5.0, 100.0, 80.0);

    EXPECT_DOUBLE_EQ(p.x, 5.0);
    EXPECT_DOUBLE_EQ(p.y, 5.0);
    EXPECT_DOUBLE_EQ(velocity.x, 3.0);
    EXPECT_DOUBLE_EQ(velocity.y, 4.0);
}

// The ball is not outside any bound.
TEST (test_rebound, no_border) {
    point p = {30.0, 30.0};
    point velocity = {3.0, 1.0};

    rebound_effect(p, velocity, 5.0, 100.0, 80.0);

    EXPECT_DOUBLE_EQ(p.x, 30.0);
    EXPECT_DOUBLE_EQ(velocity.x, 3.0);
    EXPECT_DOUBLE_EQ(p.y, 30.0);
    EXPECT_DOUBLE_EQ(velocity.y, 1.0);
}

