#ifndef SOA_HPP
#define SOA_HPP

#include "point.hpp"
#include "common.hpp"
#include <vector>
#include <limits>

struct ParticleSOA {
    std::vector<std::size_t> id;
    std::vector<double> mass;
    std::vector<double> radius;
    std::vector<common::point> position;
    std::vector<common::point> velocity;
    
    void remove_unused();

    explicit ParticleSOA(const std::vector<std::vector<double>>& particles);
};

#endif
