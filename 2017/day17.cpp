#include <algorithm>
#include <fstream>

#include "abstract_solution.h"
#include "file_input.h"

class Day17FileParser : IFileParser<size_t> {
public:
    using IFileParser<size_t>::IFileParser;

    size_t parse() const {
        std::ifstream input_file(this->file_path);
        std::string line;
        std::getline(input_file, line);

        return (size_t) std::stoi(line);
    }
};

class Day17Solution {
    const size_t NUM_STEPS = 2017;
public:
    void part1(const size_t& offset) const {
        std::vector<size_t> circular_buffer{0};

        size_t current_pos = 0;
        for (size_t i = 0; i < NUM_STEPS; i++) {
            current_pos = (current_pos + offset) % circular_buffer.size();
            // perform the insertion!
            circular_buffer.insert(circular_buffer.begin() + current_pos + 1, i+1);
            current_pos += 1;
        }

        // b/c it inserts AFTER the current posa
        current_pos = (current_pos + 1) % circular_buffer.size();

        std::cout << "(part1) item_after_insert_2017: " << circular_buffer[current_pos] << std::endl;
    }

    void part2(const size_t& offset) {
        const size_t FIFTY_MILLION = 50 * 1000 * 1000;

        size_t item_after_0 = 0;
        size_t current_pos = 0;
        for (size_t i = 0; i < FIFTY_MILLION; i++) {
            size_t buffer_size = i + 1;
            const size_t NUM_STEPS = 20;
            if ((buffer_size - current_pos) > (NUM_STEPS+1) * offset) {
                current_pos = current_pos + NUM_STEPS * offset + NUM_STEPS;
                i += NUM_STEPS-1;
                buffer_size = i + 1;
            } else {
                current_pos = (current_pos + offset) % buffer_size;
                if (current_pos == 0) {
                    item_after_0 = buffer_size;
                }
                current_pos += 1;
            }
        }

        std::cout << "(part2) item_after_0: " << item_after_0 << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day17_input.txt");
    Day17FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day17Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}