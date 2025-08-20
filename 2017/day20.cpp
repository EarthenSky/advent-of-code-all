#include <algorithm>
#include <array>
#include <fstream>
#include <format>
#include <regex>
#include <variant>
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
    void both_parts(const particle_list& particles) const {
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
};

int main() {
    std::filesystem::path input_path("./day20_input.txt");
    Day20FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day20Solution solution;
    solution.both_parts(data);
    return 0;
}