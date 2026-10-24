#ifndef SOA_COLLISIONS_HPP
#define SOA_COLLISIONS_HPP

#include <optional>
#include <utility>
#include "soa.hpp"
#include "collision.hpp"

namespace soa {
    // Create the function find_collision(), which will determine whether two particles
    // have collided or not. 
    [[nodiscard]] inline std::vector<std::size_t> find_collision(ParticleSOA const& particles) {

        std::size_t const n = particles.position.size();

        // To see whether the particles collide, we will use are_colliding() from the common namespace.
        for (std::size_t i = 0; i < n; i++) {
            for (std::size_t j = i + 1; j < n; j++){
                if (common::are_colliding(particles.position.at(i), particles.radius.at(i),particles.position.at(j), particles.radius.at(j))) {
                    return {i, j};
                }   
            }
        }

        return {};
    }

    // Implement merge_particle() to perform the absortion of one particle by other particle. 
    inline void merge_particle(ParticleSOA &particles, std::size_t absorber,std::size_t absorbed) {

        // Add up the masses of both particles
        particles.mass.at(absorber) += particles.mass.at(absorbed);

        // Modify the velocity of the resulting particle by adding the velocities of the particles that collided. 
        particles.velocity.at(absorber).x += particles.velocity.at(absorber).x;
        particles.velocity.at(absorber).y += particles.velocity.at(absorber).y;

        // Since the absorbed object no longer exist, we must delete it part by part, as we are using soa- 
        const std::size_t last = particles.id.size() - 1;
        if (absorbed != last) {
            particles.id.at(absorbed) = particles.id.at(last);
            particles.mass.at(absorbed) = particles.mass.at(last);
            particles.radius.at(absorbed) = particles.radius.at(last);
            particles.position.at(absorbed) = particles.position.at(last);
            particles.velocity.at(absorbed) = particles.velocity.at(last);
        }
        particles.id.pop_back();
        particles.mass.pop_back();
        particles.radius.pop_back();
        particles.position.pop_back();
        particles.velocity.pop_back();
    }

    // Implement resolve_collisions to 
    inline void resolve_collisions(ParticleSOA &particles){

        // To find collisions we will use a while loop
        while (true) {
            // Find a collision 
            std::vector<std::size_t> const collision = find_collision(particles);

            // If no collision was found, stop the loop. 
            if (collision.empty() ) {break;} 

            // Get the indices of the colliding particles.
            std::size_t const i = collision.at(0);
            std::size_t const j = collision.at(1);

            // Get the properties needed to choose the absorber.
            common::collider const a {particles.mass.at(i), particles.radius.at(i), static_cast <std::size_t> (particles.id.at(i))};
            common::collider const b{particles.mass.at(j), particles.radius.at(j), static_cast <std::size_t> (particles.id.at(j))};

            // Merge the absorbed particle into the absorber.
            if (common::choose_absorber(a, b)) {
                merge_particle(particles, i, j);
            } else {
                merge_particle(particles, j, i);
            }

        }

    }
}

#endif
