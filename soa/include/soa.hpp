#ifndef SOA_HPP
#define SOA_HPP

#include "point.hpp"
#include <vector>
#include <limits>

inline constexpr std::size_t unused_id = std::numeric_limits<std::size_t>::max();

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
