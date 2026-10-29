#ifndef COMMON_HPP
#define COMMON_HPP

#include <string> 
#include <vector>
#include <limits>

/* Constants
 * Store commonly used constant values as constexpr
 * UNUSED_ID: designated value as std::size_t max value to optimize collision code. Represents an absorbed particle that has not yet been removed from the system
 */
namespace constants {
    inline constexpr double G = 6.67430e-11;
    inline constexpr std::size_t UNUSED_ID = std::numeric_limits<std::size_t>::max();
}

/* Parse particle line
 * Read a line from the input file
 * Checks each component against a regular expression
 * Return an empty vector in case of a parse error
 * Return a well formed vector if there is no parse error
 */
std::vector<double> parse_particle_line(const std::string& line);

/* Check bound
 * Small function to check if a particle lies within the correct bounds
 * Likely inlined by -O3 compiler flag
 */
bool check_bound(const std::vector<double>& particle, double width, double height);


/* Read particles
 * Opens and reads input file (input/input.txt)
 * Allocates a vector of vectors to store all particle data, valid or not
 * Can be optimized by inlining and reusing the same buffer for all lines
 */
std::vector<std::vector<double>> read_particles(const std::string& input_path, double width, double height);

#endif
