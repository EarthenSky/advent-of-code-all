#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <format>
#include <ranges>
#include <string>
#include <vector>

typedef std::vector<std::vector<std::string>> string_list_list;

class Solution {
public:
    void part1(const string_list_list passphrase_list) const {
        const auto is_valid_passphrase = [](auto words) {
            for (const std::string word : words) {
                if (std::count(words.begin(), words.end(), word) >= 2)
                    return false;
            }
            return true;
        };

        size_t num_valid_passphrases = 0;
        for (const auto words : passphrase_list) {
            if (is_valid_passphrase(words))
                num_valid_passphrases += 1;
        }

        std::cout << std::format("num_valid_passphrases: {}", num_valid_passphrases) << std::endl;
    }

    void part2(const string_list_list passphrase_list) const {
        const auto are_anagrams = [](const std::string& word) {
            return [&word](const std::string& other_word){ 
                if (word != other_word) {
                    for (char c : word + other_word) {
                        // TODO: pre-computing this for `word` may improve performance
                        auto count_in_word = std::count(word.begin(), word.end(), c);
                        auto count_in_other = std::count(other_word.begin(), other_word.end(), c);
                        if (count_in_word != count_in_other)
                            return false;
                    }
                }
                return true;
            };
        };

        const auto is_valid_passphrase = [&](std::vector<std::string> words) {
            for (const auto word : words) {
                if (std::count_if(words.begin(), words.end(), are_anagrams(word)) >= 2)
                    return false;
            }
            return true;
        };

        size_t num_valid_passphrases = 0;
        for (const auto words : passphrase_list) {
            if (is_valid_passphrase(words))
                num_valid_passphrases += 1;
        }

        std::cout << std::format("num_valid_passphrases: {}", num_valid_passphrases) << std::endl;
    }
};

int main() {
    std::filesystem::path file_path("day4_input.txt");
    std::ifstream input_file(file_path);

    std::string line;
    string_list_list passphrase_list;
    // the stream is converted to bool & evaluates false when badbit is set, which occurs on EOF
    while (std::getline(input_file, line)) {
        std::vector<std::string> words;
        for (auto && word : std::views::split(line, ' ')) {
            // pass an rvalue reference!
            words.push_back(
                std::string(word.begin(), word.end())
            );
        }
        passphrase_list.push_back(std::move(words));
    }

    Solution solution;
    solution.part1(passphrase_list);
    solution.part2(passphrase_list);
    return 0;
}