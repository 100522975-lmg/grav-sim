#include <gtest/gtest.h>
#include <cmath>
#include <vector>

#include "simulation.hpp"
#include "gravity.hpp"
#include "soa.hpp"

namespace {

// soa constructor helper
ParticleSOA make_particles(const std::vector<double>& masses,
                           const std::vector<common::point>& positions) {
  ParticleSOA p{std::vector<std::vector<double>>{}};
  for (std::size_t i = 0; i < masses.size(); ++i) {
    p.id.push_back(static_cast<int>(i));
    p.mass.push_back(masses.at(i));
    p.radius.push_back(0.0);
    p.position.push_back(positions.at(i));
    p.velocity.push_back({0.0, 0.0});
  }
  return p;
}

constexpr double kTol = 1e-12;

}

//  accumulate_force_pairs 

TEST(AccumulateForcePairs, EmptyInputReturnsEmptyForces) {
  const auto p = make_particles({}, {});
  const auto f = physics::accumulate_force_pairs(p);
  EXPECT_TRUE(f.empty());
}

TEST(AccumulateForcePairs, SingleParticleHasZeroForce) {
  const auto p = make_particles({5.0}, {{1.0, 2.0}});
  const auto f = physics::accumulate_force_pairs(p);
  ASSERT_EQ(f.size(), 1u);
  EXPECT_DOUBLE_EQ(f.at(0).x, 0.0);
  EXPECT_DOUBLE_EQ(f.at(0).y, 0.0);
}

TEST(AccumulateForcePairs, OutputSizeMatchesParticleCount) {
  const auto p = make_particles({1, 2, 3, 4},
                                {{0, 0}, {1, 0}, {0, 1}, {1, 1}});
  const auto f = physics::accumulate_force_pairs(p);
  EXPECT_EQ(f.size(), 4u);
}

TEST(AccumulateForcePairs, TwoParticlesMatchPairForce) {
  const common::point p0{0.0, 0.0}, p1{3.0, 4.0};
  const auto p = make_particles({2.0, 7.0}, {p0, p1});

  const auto expected = common::pair_force(2.0, 7.0, p0, p1);
  const auto f = physics::accumulate_force_pairs(p);

  EXPECT_NEAR(f.at(0).x, expected.x, kTol);
  EXPECT_NEAR(f.at(0).y, expected.y, kTol);
  EXPECT_NEAR(f.at(1).x, -expected.x, kTol);
  EXPECT_NEAR(f.at(1).y, -expected.y, kTol);
}

TEST(AccumulateForcePairs, TwoParticlesObeyNewtonsThirdLaw) {
  const auto p = make_particles({1.5, 4.0}, {{-2.0, 1.0}, {5.0, -3.0}});
  const auto f = physics::accumulate_force_pairs(p);
  EXPECT_NEAR(f.at(0).x + f.at(1).x, 0.0, kTol);
  EXPECT_NEAR(f.at(0).y + f.at(1).y, 0.0, kTol);
}

TEST(AccumulateForcePairs, ThreeParticlesMatchSumOfPairwiseForces) {
  const std::vector<double> m{1.0, 2.0, 3.0};
  const std::vector<common::point> pos{{0.0, 0.0}, {2.0, 0.0}, {0.0, 3.0}};
  const auto p = make_particles(m, pos);

  const auto f = physics::accumulate_force_pairs(p);
  ASSERT_EQ(f.size(), 3u);

  for (std::size_t i = 0; i < 3; ++i) {
    double ex = 0.0, ey = 0.0;
    for (std::size_t j = 0; j < 3; ++j) {
      if (i == j) continue;
      const auto pf = common::pair_force(m.at(i), m.at(j), pos.at(i), pos.at(j));
      ex += pf.x;
      ey += pf.y;
    }
    EXPECT_NEAR(f.at(i).x, ex, 1e-10) << "particle " << i;
    EXPECT_NEAR(f.at(i).y, ey, 1e-10) << "particle " << i;
  }
}

TEST(AccumulateForcePairs, NetForceOfIsolatedSystemIsZero) {
  const auto p = make_particles({1.0, 2.0, 3.0, 4.0},
                                {{0, 0}, {2, 1}, {-1, 3}, {4, -2}});
  const auto f = physics::accumulate_force_pairs(p);

  double sx = 0.0, sy = 0.0;
  for (const auto& fi : f) { sx += fi.x; sy += fi.y; }
  EXPECT_NEAR(sx, 0.0, 1e-10);
  EXPECT_NEAR(sy, 0.0, 1e-10);
}

TEST(AccumulateForcePairs, DoesNotModifyInput) {
  const auto p = make_particles({1.0, 2.0}, {{0.0, 0.0}, {1.0, 1.0}});
  const auto before_pos = p.position;
  const auto before_mass = p.mass;

  const auto f = physics::accumulate_force_pairs(p);
  ASSERT_EQ(f.size(), p.position.size());

  for (std::size_t i = 0; i < p.position.size(); ++i) {
    EXPECT_DOUBLE_EQ(p.position.at(i).x, before_pos.at(i).x);
    EXPECT_DOUBLE_EQ(p.position.at(i).y, before_pos.at(i).y);
    EXPECT_DOUBLE_EQ(p.mass.at(i), before_mass.at(i));
  }
}

// compute_acceleration 

TEST(ComputeAcceleration, DividesForceByMass) {
  const auto p = make_particles({2.0, 4.0}, {{0, 0}, {1, 1}});
  const std::vector<common::point> force{{6.0, -8.0}, {10.0, 2.0}};

  const auto a = physics::compute_acceleration(p, force);

  ASSERT_EQ(a.size(), 2u);
  EXPECT_DOUBLE_EQ(a.at(0).x, 3.0);
  EXPECT_DOUBLE_EQ(a.at(0).y, -4.0);
  EXPECT_DOUBLE_EQ(a.at(1).x, 2.5);
  EXPECT_DOUBLE_EQ(a.at(1).y, 0.5);
}

TEST(ComputeAcceleration, EmptyInputAndZeroForce) {
  const auto empty = make_particles({}, {});
  const std::vector<common::point> no_force;
  const auto a_empty = physics::compute_acceleration(empty, no_force);
  EXPECT_TRUE(a_empty.empty());

  const auto p = make_particles({3.0}, {{0, 0}});
  const std::vector<common::point> zero_force{{0.0, 0.0}};
  const auto a = physics::compute_acceleration(p, zero_force);
  ASSERT_EQ(a.size(), 1u);
  EXPECT_DOUBLE_EQ(a.at(0).x, 0.0);
  EXPECT_DOUBLE_EQ(a.at(0).y, 0.0);
}
