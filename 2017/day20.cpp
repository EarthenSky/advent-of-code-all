#include <algorithm>
#include <array>
#include <fstream>
//#include <format>
#include <ranges>
#include <regex>
#include <cctype>
#include <cmath>

#include "abstract_solution.h"
#include "file_input.h"

using particle_list = std::vector<std::array<int64_t, 9>>;

class Day20FileParser : IFileParser<particle_list> {
public:
    using IFileParser<particle_list>::IFileParser;

    particle_list parse() const {
        particle_list particles;

        std::regex pattern(R"(p=<([-0-9]+),([-0-9]+),([-0-9]+)>, v=<([-0-9]+),([-0-9]+),([-0-9]+)>, a=<([-0-9]+),([-0-9]+),([-0-9]+)>)");

        std::ifstream input_file(this->file_path);
        std::string line;
        while (std::getline(input_file, line)) {
            std::smatch match;
            if (std::regex_match(line, match, pattern)) {
                particles.push_back(std::array<int64_t, 9>{
                    // TODO: it might be a cool idea to create a wrapper over smatch in order to perform these conversions automagically
                    std::stoi(match[1].str()), std::stoi(match[2].str()), std::stoi(match[3].str()),
                    std::stoi(match[4].str()), std::stoi(match[5].str()), std::stoi(match[6].str()),
                    std::stoi(match[7].str()), std::stoi(match[8].str()), std::stoi(match[9].str())
                });
            } else {
                std::runtime_error("ERROR: could not match line!");
            }
        }

        return particles;
    }
};

class Day20Solution {
public:
    void part1(const particle_list& particles) const {
        // After infinity timesteps the initial position and velocity will no longer matter (10 + inf = inf); the point
        // will be wherever it's acceleration sets the velocity to. So we can determine the closest point via magnitude
        // of acceleration (smallest).

        // if there is a tie in terms of acceleration, we use the size of the velocity component that is orthogonal to
        // the acceleration -> the one with the largest magnitude of velocity in the direction of accleration will
        // always have a larger velocity

        int64_t smallest_magnitude = __INT64_MAX__;
        double target_projected_velocity_magnitude = INFINITY;
        size_t target_i = 0;

        size_t i = 0;
        for (const std::array<int64_t,9>& particle : particles) {
            int64_t squared_magnitude = particle[6]*particle[6] + particle[7]*particle[7] + particle[8]*particle[8];
            // dot(a,b) / |b| = |a| * cos(theta) -> magnitude of a in the direction of b
            double projected_velocity_magnitude = (
                particle[3]*particle[6] + particle[4]*particle[7] + particle[5]*particle[8]
            ) / std::sqrt(squared_magnitude);
            if (squared_magnitude < smallest_magnitude) {
                smallest_magnitude = squared_magnitude;
                target_i = i;
                target_projected_velocity_magnitude = projected_velocity_magnitude;
            } else if (squared_magnitude == smallest_magnitude) {
                // B/C we're using double, it's hard to measure equality; I think we can safely assume they won't be the
                // same.
                if (projected_velocity_magnitude < target_projected_velocity_magnitude) {
                    target_i = i;
                    smallest_magnitude = squared_magnitude;
                    target_projected_velocity_magnitude = projected_velocity_magnitude;
                }
            }
            i += 1;
        }

        std::cout << "(part1) : " << target_i << std::endl;
    }

    bool points_may_intersect(std::array<int64_t, 9> p1, std::array<int64_t, 9> p2) const noexcept {
        const size_t NDIM = 3;
        for (size_t axis_i = 0; axis_i < NDIM; axis_i++) {
            if (
                // position strictly greater; this is for the case where (p,v,a) are all equal in one axis; they could
                // still collide! 
                p1[0 * 3 + axis_i] > p2[0 * 3 + axis_i]
                && p1[1 * 3 + axis_i] >= p2[1 * 3 + axis_i]
                && p1[2 * 3 + axis_i] >= p2[2 * 3 + axis_i]
            ) {
                return false;
            } else if (
                p1[0 * 3 + axis_i] < p2[0 * 3 + axis_i]
                && p1[1 * 3 + axis_i] <= p2[1 * 3 + axis_i]
                && p1[2 * 3 + axis_i] <= p2[2 * 3 + axis_i]
            ) {
                return false;
            }
        }
        return true;
    }

    void part2(const particle_list& particles2) const {
        // # compute collisions!
        // two particle will not collide if (in any axis) their velocity + acceleration match, are in opposite directions, and 
        
        // IDEA 1:
        // we can improve performance by taking n steps before checking collisions, but only when the minimum distance
        // between any two points is smaller than the largest possible step... ? Or not, perhaps...
        // OR if the velocity of one is larger than the other in that axis, is further in that direction, and the
        // accelerations are equal or will continue the trend in this axis. 

        // NOTE 2:
        // The particles are in a box of radius 4000, but they have non-zero acceleration that never dampens, which
        // means that they will probably be mostly done colliding after several hundred timesteps
        //
        // Takes O(n^2) to check every pair for collisions (could probably get it done in O(n log n) using a kdtree).
        // n is 1000, so we're looking at roughly 1s per 1k timesteps... That's doable.

        particle_list particles_left = particles2;
        size_t num_escaped_particles = 0;

        size_t timestep = 0;
        while (particles_left.size() != 0) {
            std::cout << "num particles: " << particles_left.size() << std::endl;
            // look for collisions
            std::vector<size_t> garbage_list;
            for (size_t pi = 0; pi < particles_left.size(); pi++) {
                for (size_t pj = 0; pj < particles_left.size(); pj++) {
                    if (pi == pj) continue;
                    if (
                        particles_left[pi][0] == particles_left[pj][0]
                        && particles_left[pi][1] == particles_left[pj][1]
                        && particles_left[pi][2] == particles_left[pj][2]
                    ) {
                        // TODO: this is fully O(n^2) (w/ early exit), but we could probably get it down to O(n^2) 1/2!
                        garbage_list.push_back(pi);
                        break;
                    }
                }
            }

            // TODO: we can do the removal in O(n) time, rather than O(nk)
            // There's a fancy erase using find_if or something w/ ranges approach!

            // remove garbage in reverse order, so we don't mess with the index of items behind
            for (size_t garbage_i : std::ranges::reverse_view(garbage_list))
                particles_left.erase(particles_left.begin() + garbage_i);
            garbage_list.clear();

            // update particle data
            for (auto& particle : particles_left) {
                // accel
                particle[3] += particle[6];
                particle[4] += particle[7];
                particle[5] += particle[8];

                // velocity
                particle[0] += particle[3];
                particle[1] += particle[4];
                particle[2] += particle[5];
            }

            // check which particles can never run into others anymore
            for (size_t pi = 0; pi < particles_left.size(); pi++) {
                // if only 1 particle left, it will be removed!
                bool any_may_intersect = false;
                for (size_t pj = pi+1; pj < particles_left.size(); pj++) {
                    if (points_may_intersect(particles_left[pi], particles_left[pj])) {
                        // if two points may intersect we canot prune, so exit early
                        any_may_intersect = true;
                        break;
                    }
                }

                if (!any_may_intersect) {
                    garbage_list.push_back(pi);
                    num_escaped_particles += 1;
                }
            }

            for (size_t garbage_i : std::ranges::reverse_view(garbage_list))
                particles_left.erase(particles_left.begin() + garbage_i);
        }

        std::cout << "(part2) num_escaped_particles: " << num_escaped_particles << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day20_input.txt");
    Day20FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day20Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}