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

    std::vector<common::point> calculate_accelerations(const std::vector<std::vector<common::point>>& forces);
};
    

#endif
