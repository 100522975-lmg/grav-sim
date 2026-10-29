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
    
    /* Remove unused
     * Iterates over every vector
     * Removes particles marked as unused
     * This happens at the end of every iteration
     */ 
    void remove_unused();
    
    /* Constructor
     * Reserves space in every vector
     * Takes the vector of parsed input rows
     * Discards malformed particles (empty vectors) while consuming an id
     * Otherwise, adds entry to each vector in the struct
     */
    explicit ParticleSOA(const std::vector<std::vector<double>>& particles);
};

#endif
