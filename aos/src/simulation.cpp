#include "simulation.hpp"
#include "rebound.hpp"
#include "gravity.hpp"

#include <ranges>

namespace {
inline void accumulate_forces(std::span<const particle> particles, std::vector<common::point> & forces) {
    const auto pf = std::views::zip(particles, forces);
    for (const auto [i, pf_i] : pf | std::views::enumerate) {
        auto [p_i, f_i] = pf_i;

        for (const auto [p_j, f_j]: pf | std::views::drop(i+1)) {
            const auto f = common::pair_force(p_i.mass, p_j.mass, p_i.position, p_j.position);
            f_i.x += f.x; f_i.y += f.y;
            f_j.x -= f.x; f_j.y -= f.y;
        }
    }
}

inline void compute_acceleration(std::span<const particle> particles, std::span<const common::point> const forces, std::vector<common::point> & accelerations) {
    const auto all = std::views::zip(particles, forces, accelerations);
    double inv_mass = 0;
    for (auto [p_i, f, a] : all) {
        inv_mass = 1/p_i.mass;
        a.x = f.x * inv_mass;
        a.y = f.y * inv_mass;
    }
}

    inline void apply_rebound(particle & p, double width, double height){
        common::rebound_effect(p.position, p.velocity, p.radius, 
                common::point{width, height});
    }
} // anonymous namespace for internal linkage


void step(std::vector<particle> & particles, std::vector<common::point> & forces, std::vector<common::point> & accelerations, const common::program_arguments & progargs) {
    const double dt = progargs.time_step();
    const double w = progargs.width();
    const double h = progargs.height();
    forces.clear(); accelerations.clear();
    accumulate_forces(particles, forces);
    compute_acceleration(particles, forces, accelerations);
    //move every particle 
    for(auto & p:particles){
        update_velocity(p, dt);
        //eq 5 with new velocity
        update_position(p, dt);
        apply_rebound(p, w, h);
    }
}
