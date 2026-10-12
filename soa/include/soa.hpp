#ifndef SOA_HPP
#define SOA_HPP

#include "point.hpp"
#include <vector>

struct ParticleSOA {
    std::vector<double> mass;
    std::vector<double> radius;
    std::vector<common::point> x_coord;
    std::vector<common::point> y_coord;

    explicit ParticleSOA(const std::vector<std::vector<double>>& particles);
};


#endif
