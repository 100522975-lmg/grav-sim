#ifndef WRITER_HPP
#define WRITER_HPP

#include "point.hpp"
#include <cstddef>
#include <fstream>
#include <print>
#include <ranges>
#include <string>

namespace writer {
  // generic code for SOA and AOS writing
  template <std::ranges::random_access_range M,
            std::ranges::random_access_range P>
  struct write_args {
    double w, h, dt;
    M m, r;
    P p, v;
    std::size_t n;
  };

  template <class M, class P>
  write_args(double, double, double, M, M, P, P, std::size_t) -> write_args<M, P>;

  // implementation details, not part of the public interface
  namespace detail {
    struct header_args {
      double w, h, dt;
      std::size_t n;
    };

    struct line_args {
      double m, r;
      common::point p, v;
    };

    inline void format_header(const header_args& args, std::ofstream& buffer) {
      std::println(buffer, "{} {} {} {}", args.w, args.h, args.dt, args.n);
    }

    inline void format_particle_line(const line_args& args, std::ofstream& buffer) {
      std::println(buffer, "{} {} {} {} {} {}",
                   args.m, args.r, args.p.x, args.p.y, args.v.x, args.v.y);
    }
  }  // namespace detail

  template <class M, class P>
  void write_config(const write_args<M, P>& args, const std::string& path) {
    std::ofstream out{path};
    detail::format_header(detail::header_args{args.w, args.h, args.dt, args.n}, out);
    for (const auto& [m, r, p, v] : std::views::zip(args.m, args.r, args.p, args.v)) {
      detail::format_particle_line(detail::line_args{m, r, p, v}, out);
    }
  }
}  // namespace writer

#endif
