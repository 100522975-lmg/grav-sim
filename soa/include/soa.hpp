#ifndef SOA_HPP
#define SOA_HPP

#include "point.hpp"
#include <vector>


struct ParticleSOA {
    std::vector<int> id;
    std::vector<double> mass;
    std::vector<double> radius;
    std::vector<common::point> position;
    std::vector<common::point> velocity;

    explicit ParticleSOA(const std::vector<std::vector<double>>& particles);

    [[nodiscard]] std::vector<common::point> accumulate_force_pairs() const;

    [[nodiscard]] std::vector<common::point> compute_acceleration(const std::vector<std::vector<common::point>>& forces) const;

    inline void update_velocity(const std::vector<common::point>& acceleration, const double time_step);

    inline void update_position(const double time_step);

    inline void apply_rebound(const double width, const double height);
};
    

#endif
