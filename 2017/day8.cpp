#include <algorithm>
#include <format>
#include <ranges>

#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>

#include "abstract_solution.h"
#include "file_input.h"

enum LockKind {
    READ = 0,
    WRITE = 1
};

// use per-register mutexes in order to run this using 4 cores, instead of 1!
class Day8Solution_V2 {
    std::vector<std::jthread> thread_pool;

    std::unordered_map<std::string, int64_t> register_vals;

    size_t instruction_i = 0;
    std::atomic<size_t> instructions_completed = 0;
    std::mutex grab_instruction;

    // any instruction in this set is currently being held
    std::unordered_map<std::string, std::pair<LockKind, size_t>> instruction_locks;
    std::mutex update_register_locks;

    /// @brief returns whether there was a new instruction or not
    bool grab_next_instruction(
        const Day8Data& data,
        size_t& current_instruction_i,
        std::string& write_lock_str,
        std::string& read_lock_str
    ) {
        // grab an instruction
        // NOTE: no deadlock because register lock is all that's needed to release a register 
        std::lock_guard lock(grab_instruction);
        if (instruction_i == data.instruction_list.size())
            return false;

        current_instruction_i = instruction_i;
        instruction_i += 1;

        write_lock_str = data.instruction_list[current_instruction_i].target_reg;
        read_lock_str = data.instruction_list[current_instruction_i].cond_reg;
        
        while (true) {
            // grab locks, or busywait until they're both avaliable
            {
                std::lock_guard register_lock(update_register_locks);

                if (
                    (!instruction_locks.contains(write_lock_str))
                    && (!instruction_locks.contains(read_lock_str) || instruction_locks[read_lock_str].first == READ)
                ) {
                    // TODO: this must be atomic, so rollback on a hypothetical failure
                    if (instruction_locks[read_lock_str].first == READ)
                        instruction_locks[read_lock_str].second += 1;
                    else
                        instruction_locks[write_lock_str] = std::pair(READ, 1);
                    
                    instruction_locks[write_lock_str] = std::pair(WRITE, 1);
                    break;
                }
            }

            using us = std::chrono::duration<size_t, std::micro>;
            std::this_thread::sleep_for(us(5));
        }

        return true;
    }

    // LOTS OF OVERHEAD!
    void perform_register_operations(const Day8Data& data) {
        while (true) {
            size_t current_instruction_i;
            std::string write_lock_str;
            std::string read_lock_str;
            if (!this->grab_next_instruction(
                data, current_instruction_i, write_lock_str, read_lock_str
            ))
                return;

            // evaluate target register
            const Day8Instruction& inst = data.instruction_list[current_instruction_i];
            if (
                (inst.cond == ">" && register_vals[inst.cond_reg] > inst.cond_val)
                || (inst.cond == ">=" && register_vals[inst.cond_reg] >= inst.cond_val)
                || (inst.cond == "<" && register_vals[inst.cond_reg] < inst.cond_val)
                || (inst.cond == "<=" && register_vals[inst.cond_reg] <= inst.cond_val)
                || (inst.cond == "==" && register_vals[inst.cond_reg] == inst.cond_val)
                || (inst.cond == "!=" && register_vals[inst.cond_reg] != inst.cond_val)
            ) {
                register_vals[inst.target_reg] += inst.offset;
            }

            // remove our locks
            {
                // must also be atomic
                std::lock_guard register_lock(update_register_locks);
                if (instruction_locks[read_lock_str].second > 1)
                    instruction_locks[read_lock_str].second -= 1;
                else
                    instruction_locks.erase(read_lock_str);

                instruction_locks.erase(write_lock_str);
            }

            instructions_completed += 1;
        }
    }

public:
    void part1(const Day8Data& data) {
        this->register_vals = data.register_vals;

        for (size_t ti = 0; ti < 4; ti++)
            thread_pool.push_back(
                std::move(
                    std::jthread(
                        &Day8Solution_V2::perform_register_operations,
                        this,
                        std::cref(data)
                    )
                )
            );

        // TODO: how to use events instead of busywaiting?
        while (instructions_completed != data.instruction_list.size()) {
            using ms = std::chrono::duration<size_t, std::milli>;
            std::this_thread::sleep_for(ms(1));
        }

        for (auto& t : thread_pool)
            t.join();

        // find max value in registers using fancy c++20 ranges!
        int64_t max_element = std::ranges::max_element(this->register_vals, {}, [](const auto& keyval) {
            return keyval.second;
        })->second;

        std::cout << std::format("(part1_v21) max_element: {}", max_element) << std::endl;
    }
};

class Day8Solution : ISolution<Day8Data> {
    friend int main();

    void part1(const Day8Data& data) const {
        // evaluate registers
        std::unordered_map<std::string, int64_t> register_vals = data.register_vals;
        for (const Day8Instruction& inst : data.instruction_list) {
            if (
                (inst.cond == ">" && register_vals[inst.cond_reg] > inst.cond_val)
                || (inst.cond == ">=" && register_vals[inst.cond_reg] >= inst.cond_val)
                || (inst.cond == "<" && register_vals[inst.cond_reg] < inst.cond_val)
                || (inst.cond == "<=" && register_vals[inst.cond_reg] <= inst.cond_val)
                || (inst.cond == "==" && register_vals[inst.cond_reg] == inst.cond_val)
                || (inst.cond == "!=" && register_vals[inst.cond_reg] != inst.cond_val)
            ) {
                // std::cout << inst.offset << std::endl;
                register_vals[inst.target_reg] += inst.offset;
            }
        }

        // find max value in registers using fancy c++20 ranges!
        int64_t max_element = std::ranges::max_element(register_vals, {}, [](const auto& keyval) {
            return keyval.second;
        })->second;

        // TODO: use per-register mutexes in order to run this using 4 cores, instead of 1!

        std::cout << std::format("(part1) max_element: {}", max_element) << std::endl;
    }

    void part2(const Day8Data& data) const {
        // evaluate registers
        int64_t max_value_ever = 0;
        std::unordered_map<std::string, int64_t> register_vals = data.register_vals;
        // TODO: use per-register mutexes in order to run THIS using 4 cores, instead of 1!
        for (const Day8Instruction& inst : data.instruction_list) {
            if (
                (inst.cond == ">" && register_vals[inst.cond_reg] > inst.cond_val)
                || (inst.cond == ">=" && register_vals[inst.cond_reg] >= inst.cond_val)
                || (inst.cond == "<" && register_vals[inst.cond_reg] < inst.cond_val)
                || (inst.cond == "<=" && register_vals[inst.cond_reg] <= inst.cond_val)
                || (inst.cond == "==" && register_vals[inst.cond_reg] == inst.cond_val)
                || (inst.cond == "!=" && register_vals[inst.cond_reg] != inst.cond_val)
            ) {
                // std::cout << inst.offset << std::endl;
                register_vals[inst.target_reg] += inst.offset;
                max_value_ever = std::max(register_vals[inst.target_reg], max_value_ever);
            }
        }

        std::cout << std::format("(part2) max_value_ever: {}", max_value_ever) << std::endl;
    }
};

int main() {
    std::filesystem::path input_path("./day8_input.txt");
    InputFile input_file(std::move(input_path));
    Day8Data data = input_file.parse_day8();

    Day8Solution solution;
    solution.part1(data);
    solution.part2(data);

    Day8Solution_V2 solution2;
    solution2.part1(data);
    return 0;
}