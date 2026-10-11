#ifndef SOA_HPP
#define SOA_HPP

#include "point.hpp"
#include <vector>

struct ParticleSOA {
    std::vector<double> mass;
    std::vector<double> radius;
    std::vector<point> x_coord;
    std::vector<point> y_coord;

    explicit ParticleSOA(const std::vector<std::vector<double>>& particles);
}


#endif
