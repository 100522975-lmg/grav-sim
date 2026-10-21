#ifndef SOA_COLLISIONS_HPP
#define SOA_COLLISIONS_HPP

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "soa.hpp"
#include "collision.hpp"

// Use namespace std to avoid writing std:: all the time. 
using namespace std;

namespace soa {
    // Create the function find_collision(), which will determine whether two particles
    // have collided or not. 
    [[nodiscard]] inline vector<size_t> find_collision(ParticleSOA const& particles) {

        size_t n = particles.position.size();

        // To see whether the particles collide, we will use are_colliding() from the common namespace.
        for (size_t i = 0; i < n; i++) {
            for (size_t j = i + 1; j < n; j++){
                if (common::are_colliding(particles.position[i], particles.radius[i],particles.position[j], particles.radius[j])) {
                    return {i, j};
                }   
            }
        }

        return {};
    }

    // Implement merge_particle() to perform the absortion of one particle by other particle. 
    inline void merge_particle(ParticleSOA &particles, size_t absorber,size_t absorbed) {

        // Add up the masses of both particles
        particles.mass[absorber] += particles.mass[absorbed];

        // Modify the velocity of the resulting particle by adding the velocities of the particles that collided. 
        particles.velocity[absorber].x += particles.velocity[absorbed].x;
        particles.velocity[absorber].y += particles.velocity[absorbed].y;

        // Since the absorbed object no longer exist, we must delete it part by part, as we are using soa- 
        particles.id.erase(particles.id.begin() + absorbed);
        particles.mass.erase(particles.mass.begin() + absorbed);
        particles.radius.erase(particles.radius.begin() + absorbed);
        particles.position.erase(particles.position.begin() + absorbed);
        particles.velocity.erase(particles.velocity.begin() + absorbed);
    }

    // Implement resolve_collisions to 
    inline void resolve_collisions(ParticleSOA &particles){

        // To find collisions we will use a while loop
        while (true) {
            // Find a collision 
            vector<size_t> const collision = find_collision(particles);

            // If no collision was found, stop the loop. 
            if (collision.empty() == true) {break;} 

            // Get the indices of the colliding particles.
            size_t const i = collision[0];
            size_t const j = collision[1];

            // Get the properties needed to choose the absorber.
            common::collider const a {particles.mass[i], particles.radius[i], static_cast <size_t> (particles.id[i])};
            common::collider const b{particles.mass[j], particles.radius[j], static_cast <size_t> (particles.id[j])};

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
