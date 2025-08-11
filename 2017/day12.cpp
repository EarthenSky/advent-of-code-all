#include <algorithm>
#include <format>
#include <fstream>
#include <ranges>

#include <unordered_set>

#include "abstract_solution.h"
#include "file_input.h"

typedef std::vector<std::vector<size_t>> vector_graph;

class Day12FileParser : IFileParser<vector_graph> {
public:
    using IFileParser<vector_graph>::IFileParser;

    vector_graph parse() const {
        std::ifstream input_file(this->file_path);
        vector_graph graph;

        std::string line;
        while (std::getline(input_file, line)) {
            auto it = std::views::split(line, std::string(" <-> ")).begin();
            it++;
            // assume LHS is in order
            // std::string first_str(it.begin(), it.end());
            std::string second_str((*it).begin(), (*it).end());

            graph.push_back(std::vector<size_t>());
            for (auto&& num_view : std::views::split(second_str, std::string(", "))) {
                graph.back().push_back(
                    (size_t) std::stoi(std::string(num_view.begin(), num_view.end()))
                );
            }
        }

        return graph;
    }
};


class Day12Solution : ISolution<vector_graph> {
public:
    void part1(const vector_graph& graph) const {
        std::vector<size_t> frontier;
        // TODO: use a boolean vector of size 2000 for visited
        std::unordered_set<size_t> visited;
        frontier.push_back(0);

        while (!frontier.empty()) {
            size_t next_node = frontier.back();
            frontier.pop_back();
            visited.insert(next_node);

            for (size_t incident_node : graph[next_node]) {
                if (!visited.contains(incident_node))
                    frontier.push_back(incident_node);
            }
        }

        std::cout << std::format("(part1) size of id 0 group: {}", visited.size()) << std::endl;        
    }
    void part2(const vector_graph& graph) const {
        std::unordered_set<size_t> unvisited;
        for (size_t i = 0; i < graph.size(); i++)
            unvisited.insert(i);

        size_t num_groups = 0;
        while (!unvisited.empty()) {
            std::vector<size_t> frontier;
            std::unordered_set<size_t> visited;
            frontier.push_back(*unvisited.begin());

            while (!frontier.empty()) {
                size_t next_node = frontier.back();
                frontier.pop_back();
                // std::cout << "contains: " << next_node << std::endl;
                visited.insert(next_node);

                for (size_t incident_node : graph[next_node]) {
                    if (!visited.contains(incident_node))
                        frontier.push_back(incident_node);
                }
            }

            num_groups += 1;
            std::erase_if(
                unvisited,
                [&visited](size_t node) { return visited.contains(node); }
            );
        }

        std::cout << std::format("(part2) num_groups: {}", num_groups) << std::endl;   
    }
};

int main() {
    std::filesystem::path input_path("./day12_input.txt");
    Day12FileParser parser(std::move(input_path));
    auto data = parser.parse();

    Day12Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}