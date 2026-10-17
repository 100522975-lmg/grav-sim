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

    [[nodiscard]] std::vector<common::point> compute_acceleration(const std::vector<common::point>& force) const;
    
    void update_all(const std::vector<common::point>& acceleration, double dt);

    void apply_rebound(const double width, const double height);
};
    

#endif
