#include <format>

#include "file_input.h"
#include "abstract_solution.h"

struct Data {
    std::unordered_map<std::string, std::vector<std::string>> parent_child_mapping;
    std::unordered_map<std::string, std::string> child_parent_mapping;
    std::unordered_map<std::string, size_t> weight;
};

class Day7Solution : ISolution<Data> {
    friend int main();

    void part1(const Data& data) const {
        for (const auto& [name, weight] : data.weight) {
            if (!data.child_parent_mapping.contains(name)) {
                std::cout << std::format("root_name: {}", name) << std::endl;
                break;
            }
        }
    }

    size_t weight_of_tree(const Data& data, const std::string& name) const {
        size_t total_weight = (*data.weight.find(name)).second;

        if (data.parent_child_mapping.contains(name)) {
            auto children = (*data.parent_child_mapping.find(name)).second;
            for (const std::string& child : children) {
                total_weight += weight_of_tree(data, child);
            }
        }

        return total_weight;
    }

    bool is_unbalanced(const Data& data, const std::string& name) const {
        if (!data.parent_child_mapping.contains(name))
            return false;

        bool has_weight_been_set = false;        
        size_t last_weight = 0;
        for (const std::string& child : data.parent_child_mapping.find(name)->second) {
            if (!has_weight_been_set) {
                last_weight = weight_of_tree(data, child);
                has_weight_been_set = true;
            } else {
                size_t new_weight = weight_of_tree(data, child);
                if (last_weight != new_weight)
                    return true;
            }
        }
        return false;
    }

    const std::string& find_unbalanced_node(
        const Data& data,
        int64_t& offset,
        const std::string& name
    ) const {
        if (is_unbalanced(data, name)) {
            auto& children = data.parent_child_mapping.find(name)->second;

            if (children.size() == 2) {
                throw std::runtime_error("unbalanced node is not clear, because it has only 2 children");
            } else if (children.size() < 2) {
                throw std::runtime_error("did not expect fewer than 2 children");
            }

            size_t weight_a = weight_of_tree(data, children[0]);
            size_t num_a = 0;
            size_t num_other = 0;

            size_t i_of_other;
            size_t weight_of_other;
            size_t i = 0;
            for (const std::string& child : children) {
                size_t child_weight = weight_of_tree(data, child);
                if (child_weight == weight_a) {
                    num_a += 1;
                } else {
                    num_other += 1;
                    i_of_other = i;
                    weight_of_other = child_weight;
                }

                i += 1;
            }

            if (num_a == 1) {
                offset = ((int64_t) weight_of_other) - ((int64_t) weight_a);
                return find_unbalanced_node(data, offset, children[0]);
            } else if (num_other == 1) {
                offset = ((int64_t) weight_a) - ((int64_t) weight_of_other);
                return find_unbalanced_node(data, offset, children[i_of_other]);
            } else {
                throw std::runtime_error("unexpected sums...");
            }
        } else {
            // if my children are not unbalanced, I must be the wrong weight!
            return name;
        }
    }

    void part2(const Data& data) const {
        std::string root;
        for (const auto& [name, weight] : data.weight) {
            if (!data.child_parent_mapping.contains(name)) {
                root = name;
                break;
            }
        }

        int64_t offset = 0;
        std::string unbalanced_node = find_unbalanced_node(data, offset, root);
        std::cout << "for:\t\t" << unbalanced_node << "\n";
        std::cout << "offset: \t" << offset << "\n";
        std::cout << "new_value:\t" << ((int64_t) data.weight.find(unbalanced_node)->second + offset) << "\n";
    }
};

int main() {
    std::filesystem::path input_path("./day7_input.txt");
    InputFile file_input(std::move(input_path));

    Data data;
    file_input.parse_day7(data.parent_child_mapping, data.child_parent_mapping, data.weight);

    Day7Solution solution;
    solution.part1(data);
    solution.part2(data);
    return 0;
}