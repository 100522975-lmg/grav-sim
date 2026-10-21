#include "common.hpp"
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>



// Test: a valid line gives the six values in the order mass, radius, x, y, vx, vy.
TEST(test_common, parse_valid_line) {
    auto const row = parse_particle_line("10.5 2.0 30.25 40.0 1.5 2.5");

    ASSERT_EQ(row.size(), 6U);
    EXPECT_DOUBLE_EQ(row.at(0), 10.5);
    EXPECT_DOUBLE_EQ(row.at(1), 2.0);
    EXPECT_DOUBLE_EQ(row.at(2), 30.25);
    EXPECT_DOUBLE_EQ(row.at(3), 40.0);
    EXPECT_DOUBLE_EQ(row.at(4), 1.5);
    EXPECT_DOUBLE_EQ(row.at(5), 2.5);
}

// Test: the velocity components can be negative.
TEST(test_common, parse_negative_velocities) {
    auto const row = parse_particle_line("1.0 1.0 5.0 5.0 -1.5 -2.5");

    ASSERT_EQ(row.size(), 6U);
    EXPECT_DOUBLE_EQ(row.at(4), -1.5);
    EXPECT_DOUBLE_EQ(row.at(5), -2.5);
}

// Test: a file with Windows line endings leaves a '\r' at the end of the line.
TEST(test_common, parse_trailing_carriage_return) {
    auto const row = parse_particle_line("1.0 1.0 5.0 5.0 1.0 1.0\r");

    EXPECT_EQ(row.size(), 6U);
}

// Test: a negative mass is not a valid line.
TEST(test_common, parse_negative_mass) {
    EXPECT_TRUE(parse_particle_line("-1.0 1.0 5.0 5.0 1.0 1.0").empty());
}

// Test: a negative radius is not a valid line.
TEST(test_common, parse_negative_radius) {
    EXPECT_TRUE(parse_particle_line("1.0 -1.0 5.0 5.0 1.0 1.0").empty());
}

// Test: a negative position is not a valid line (x and y).
TEST(test_common, parse_negative_position) {
    EXPECT_TRUE(parse_particle_line("1.0 1.0 -5.0 5.0 1.0 1.0").empty());
    EXPECT_TRUE(parse_particle_line("1.0 1.0 5.0 -5.0 1.0 1.0").empty());
}

// Test: five values instead of six.
TEST(test_common, parse_too_few_values) {
    EXPECT_TRUE(parse_particle_line("1.0 1.0 5.0 5.0 1.0").empty());
}

// Test: seven values instead of six.
TEST(test_common, parse_too_many_values) {
    EXPECT_TRUE(parse_particle_line("1.0 1.0 5.0 5.0 1.0 1.0 1.0").empty());
}

// Test: text instead of a number.
TEST(test_common, parse_non_numeric_value) {
    EXPECT_TRUE(parse_particle_line("1.0 abc 5.0 5.0 1.0 1.0").empty());
}

// Test: empty line.
TEST(test_common, parse_empty_line) {
    EXPECT_TRUE(parse_particle_line("").empty());
}

// Test: the last number is cut ("1." instead of "1.0").
TEST(test_common, parse_incomplete_number) {
    EXPECT_TRUE(parse_particle_line("1.0 1.0 5.0 5.0 1.0 1.").empty());
}

// Boundary tests use the particle format:
// {mass, radius, x, y, vx, vy}

// Test: particle in the middle of the enclosure.
TEST(test_common, bound_inside) {
    EXPECT_TRUE(check_bound({1.0, 2.0, 50.0, 25.0, 0.0, 0.0}, 100.0, 50.0));
}

// Test: touching the left wall (x - radius = 0) is still inside.
TEST(test_common, bound_touching_left_wall) {
    EXPECT_TRUE(check_bound({1.0, 2.0, 2.0, 25.0, 0.0, 0.0}, 100.0, 50.0));
}

// Test: touching the right wall (x + radius = width) is still inside.
TEST(test_common, bound_touching_right_wall) {
    EXPECT_TRUE(check_bound({1.0, 2.0, 98.0, 25.0, 0.0, 0.0}, 100.0, 50.0));
}

// Test: touching the bottom wall (y - radius = 0) is still inside.
TEST(test_common, bound_touching_bottom_wall) {
    EXPECT_TRUE(check_bound({1.0, 2.0, 50.0, 2.0, 0.0, 0.0}, 100.0, 50.0));
}

// Test: touching the top wall (y + radius = height) is still inside.
TEST(test_common, bound_touching_top_wall) {
    EXPECT_TRUE(check_bound({1.0, 2.0, 50.0, 48.0, 0.0, 0.0}, 100.0, 50.0));
}

// Test: crossing the left wall.
TEST(test_common, bound_crossing_left_wall) {
    EXPECT_FALSE(check_bound({1.0, 2.0, 1.9, 25.0, 0.0, 0.0}, 100.0, 50.0));
}

// Test: crossing the right wall.
TEST(test_common, bound_crossing_right_wall) {
    EXPECT_FALSE(check_bound({1.0, 2.0, 98.1, 25.0, 0.0, 0.0}, 100.0, 50.0));
}

// Test: crossing the bottom wall.
TEST(test_common, bound_crossing_bottom_wall) {
    EXPECT_FALSE(check_bound({1.0, 2.0, 50.0, 1.9, 0.0, 0.0}, 100.0, 50.0));
}

// Test: crossing the top wall.
TEST(test_common, bound_crossing_top_wall) {
    EXPECT_FALSE(check_bound({1.0, 2.0, 50.0, 48.1, 0.0, 0.0}, 100.0, 50.0));
}

// Test: x is valid but y is not.
TEST(test_common, bound_x_valid_y_invalid) {
    EXPECT_FALSE(check_bound({1.0, 2.0, 50.0, 60.0, 0.0, 0.0}, 100.0, 50.0));
}

// Test: the width limits x and the height limits y.
TEST(test_common, bound_uses_width_for_x_and_height_for_y) {
    EXPECT_TRUE(check_bound({1.0, 2.0, 60.0, 10.0, 0.0, 0.0}, 100.0, 50.0));
    EXPECT_FALSE(check_bound({1.0, 2.0, 60.0, 10.0, 0.0, 0.0}, 50.0, 50.0));
    EXPECT_FALSE(check_bound({1.0, 2.0, 10.0, 60.0, 0.0, 0.0}, 100.0, 50.0));
}

// Test: a particle bigger than the enclosure never fits.
TEST(test_common, bound_particle_bigger_than_enclosure) {
    EXPECT_FALSE(check_bound({1.0, 40.0, 25.0, 25.0, 0.0, 0.0}, 50.0, 50.0));
}

// Test: a row with fewer than four values throws.
TEST(test_common, bound_short_row) {
    EXPECT_THROW((void) check_bound({1.0, 2.0}, 100.0, 50.0), std::out_of_range);
}

// File-reading tests use unique temporary filenames.
// The cleanup callback removes each file when the test finishes.
namespace {
    auto make_test_file(std::string const& content) {
        static int counter = 0;
        auto const path = std::filesystem::temp_directory_path()
            / ("grav_test_common_" + std::to_string(counter++) + ".txt");

        {
            std::ofstream out{path, std::ios::binary};
            out << content;
        }

        return path;
    }
}

// Test: valid particles are kept in the order of the file.
TEST(test_common, read_valid_particles_in_file_order) {
    auto const path = make_test_file(
        "1.0 1.0 10.0 10.0 0.5 0.5\n"
        "2.0 1.0 20.0 20.0 -1.0 1.0\n");

    auto const particles = read_particles(path.string(), 100.0, 100.0);
    std::filesystem::remove(path);

    ASSERT_EQ(particles.size(), 2U);
    EXPECT_DOUBLE_EQ(particles.at(0).at(0), 1.0);
    EXPECT_DOUBLE_EQ(particles.at(1).at(0), 2.0);
    EXPECT_DOUBLE_EQ(particles.at(1).at(4), -1.0);
}

// Test: lines with a wrong format (text, empty line) are skipped.
TEST(test_common, read_skips_lines_with_wrong_format) {
    auto const path = make_test_file(
        "1.0 1.0 10.0 10.0 0.5 0.5\n"
        "this is not a particle\n\n"
        "2.0 1.0 20.0 20.0 0.0 0.0\n");

    auto const particles = read_particles(path.string(), 100.0, 100.0);
    std::filesystem::remove(path);

    EXPECT_EQ(particles.size(), 2U);
}

// Test: mass 0 is discarded.
TEST(test_common, read_discards_zero_mass) {
    auto const path = make_test_file("0.0 1.0 10.0 10.0 0.0 0.0\n");
    auto const particles = read_particles(path.string(), 100.0, 100.0);
    std::filesystem::remove(path);

    EXPECT_TRUE(particles.empty());
}

// Test: radius 0 is discarded.
TEST(test_common, read_discards_zero_radius) {
    auto const path = make_test_file("1.0 0.0 10.0 10.0 0.0 0.0\n");
    auto const particles = read_particles(path.string(), 100.0, 100.0);
    std::filesystem::remove(path);

    EXPECT_TRUE(particles.empty());
}

// Test: a particle that crosses a wall is discarded and the valid one is kept.
TEST(test_common, read_discards_particles_outside_enclosure) {
    auto const path = make_test_file(
        "1.0 5.0 3.0 50.0 0.0 0.0\n"
        "1.0 5.0 50.0 50.0 0.0 0.0\n");

    auto const particles = read_particles(path.string(), 100.0, 100.0);
    std::filesystem::remove(path);

    ASSERT_EQ(particles.size(), 1U);
    EXPECT_DOUBLE_EQ(particles.at(0).at(2), 50.0);
}

// Test: the same particle is valid in a big enclosure and invalid in a small one.
TEST(test_common, read_uses_the_given_enclosure_size) {
    auto const path = make_test_file("1.0 1.0 60.0 60.0 0.0 0.0\n");

    EXPECT_EQ(read_particles(path.string(), 100.0, 100.0).size(), 1U);
    EXPECT_TRUE(read_particles(path.string(), 50.0, 50.0).empty());

    std::filesystem::remove(path);
}

// Test: an incomplete last line is ignored.
TEST(test_common, read_ignores_incomplete_last_line) {
    auto const path = make_test_file(
        "1.0 1.0 10.0 10.0 0.5 0.5\n"
        "2.0 1.0 20.0 20.0 -1.");

    auto const particles = read_particles(path.string(), 100.0, 100.0);
    std::filesystem::remove(path);

    EXPECT_EQ(particles.size(), 1U);
}

// Test: a complete last line is accepted even without the final line break.
TEST(test_common, read_accepts_complete_last_line_without_newline) {
    auto const path = make_test_file(
        "1.0 1.0 10.0 10.0 0.5 0.5\n"
        "2.0 1.0 20.0 20.0 1.0 1.0");

    auto const particles = read_particles(path.string(), 100.0, 100.0);
    std::filesystem::remove(path);

    EXPECT_EQ(particles.size(), 2U);
}

// Test: Windows line endings (\r\n).
TEST(test_common, read_windows_line_endings) {
    auto const path = make_test_file(
        "1.0 1.0 10.0 10.0 0.5 0.5\r\n"
        "2.0 1.0 20.0 20.0 1.0 1.0\r\n");

    auto const particles = read_particles(path.string(), 100.0, 100.0);
    std::filesystem::remove(path);

    EXPECT_EQ(particles.size(), 2U);
}

// Test: empty file.
TEST(test_common, read_empty_file) {
    auto const path = make_test_file("");
    auto const particles = read_particles(path.string(), 100.0, 100.0);
    std::filesystem::remove(path);

    EXPECT_TRUE(particles.empty());
}

// Test: the file does not exist.
TEST(test_common, read_missing_file) {
    EXPECT_TRUE(read_particles(
        "this/file/does/not/exist.txt", 100.0, 100.0).empty());
}