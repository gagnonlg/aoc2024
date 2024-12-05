#pragma once

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <format>
#include <iostream>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

using Answer = int64_t;

std::vector<std::string> split_at(std::istream& stream, char c, bool allow_empty = false);
std::vector<std::string> split_at(const std::string& str, char c, bool allow_empty = false);
std::vector<std::string> split_at(const char *str, char c, bool allow_empty = false);

std::vector<std::string> split_lines(std::istream& stream);
std::vector<std::string> split_lines(const std::string& str);
std::vector<std::string> split_lines(const char *str);

std::vector<int64_t> split_ints(std::istream& stream, char c  = ' ');
std::vector<int64_t> split_ints(const std::string& str, char c  = ' ');
std::vector<int64_t> split_ints(const char *str, char c  = ' ');

std::vector<std::vector<std::string>> split_columns(std::istream& stream);
std::vector<std::vector<std::string>> split_columns(const std::string& str);
std::vector<std::vector<std::string>> split_columns(const char *str);

std::vector<std::vector<int64_t>> split_int_columns(std::istream& stream);
std::vector<std::vector<int64_t>> split_int_columns(const std::string& str);
std::vector<std::vector<int64_t>> split_int_columns(const char *str);

std::vector<std::vector<int64_t>> split_int_lines(std::istream& stream);
std::vector<std::vector<int64_t>> split_int_lines(const std::string& str);
std::vector<std::vector<int64_t>> split_int_lines(const char *str);


#define CHECK(cond) do { if (!(cond)) { throw std::runtime_error(std::format("Assertion error in file {} at line {}: {}", __FILE__, __LINE__, #cond)); } } while (0)


class NotImplemented : std::exception {};

template <int N>
struct Day : std::false_type {};

template <int N>
struct DayBase  {
    static constexpr bool value = true;

    virtual void tests() = 0;
    virtual Answer part_1(const std::string&) = 0;
    virtual Answer part_2(const std::string&) = 0;

    virtual ~DayBase() = default;

    std::string read_to_string(const std::string& path) {
	std::ifstream ifile(path);
	if (ifile) {
	    std::stringstream buffer;
	    buffer << ifile.rdbuf();
	    return buffer.str();
	}
	throw std::runtime_error("Unable to open " + path);
    }

    std::string load_input() {
	return read_to_string("inputs/day" + std::to_string(N) + ".txt");
    }

    void verify_print(Answer answer, int part) {
	std::cout << answer << std::flush;

	Answer correct;
	try {
	    correct = std::stoll(
		read_to_string("inputs/day" +  std::to_string(N)
			       + "_" + std::to_string(part) + ".txt"));
	} catch(std::exception&) {
	    std::cout << " (?)" << std::endl;
	    return;
	}

	std::cout << std::endl;

	if (correct == answer) {
	} else {
	    throw std::runtime_error(
		"Answer given: " + std::to_string(answer)
		+ " does not match known answer: " + std::to_string(correct));
	}
    }
    
    int run(std::optional<int> part) {
	std::cout << "Day " << N << ", Tests  // " << std::flush;
	try {
	    tests();
	    std::cout << "PASS" << std::endl;
	} catch(NotImplemented&) {
	    std::cout << "Not Implemented" << std::endl;
	}

	std::string input = load_input();

	if (!part || *part == 1) {
	    std::cout << "Day " << N << ", Part 1 // " << std::flush;
	    try {
		Answer ans_1 = part_1(input);
		verify_print(ans_1, 1);
	    } catch(NotImplemented&) {
		std::cout << "Not Implemented" << std::endl;
	    }
	}

	if (!part || *part == 2) {
	    std::cout << "Day " << N << ", Part 2 // " << std::flush;
	    try {
		Answer ans_2 = part_2(input);
		verify_print(ans_2, 2);
	    } catch(NotImplemented&) {
		std::cout << "Not Implemented" << std::endl;
	    }
	}

	return EXIT_SUCCESS;
    }
};


#define DAY(N)								\
    namespace day##N {							\
    void tests();							\
    Answer part_1(const std::string&);					\
    Answer part_2(const std::string&);					\
    }									\
    template <> struct Day<N> : DayBase<N> {				\
	void tests() override { day##N::tests(); }			\
	Answer part_1(const std::string& input) override {		\
	    return day##N::part_1(input);				\
	}								\
	Answer part_2(const std::string& input) override {		\
	    return day##N::part_2(input);				\
	}								\
    }


template <typename T>
inline
Answer ans(T val);

template <>
inline
Answer ans(int64_t val)
{
    return val;
}

template <>
inline
Answer ans(uint64_t val)
{
    if (val > std::numeric_limits<Answer>::max()) {
	throw std::runtime_error("Invalid unsigned -> signed conversion");
    }
    return static_cast<Answer>(val);
}
