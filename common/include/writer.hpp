#ifndef WRITER_HPP
#define WRITER_HPP

#include "point.hpp"
#include <fstream>
#include <print>

namespace common {
  struct writer {
    std::ofstream buffer; 
   
    explicit writer(const std::string& path) : buffer(path) {}

    void format_header(double w, double h, double dt, std::size_t n) {
      std::println(buffer, "{} {} {} {}", w, h, dt, n);
    }

    void format_particle_line(double m, double r, common::point p, common::point v) {
      std::println(buffer, "{} {} {} {} {} {}", m, r, p.x, p.y, v.x, v.y);
    }
  };
}

#endif
