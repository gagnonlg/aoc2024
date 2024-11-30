#include <optional>
#include <stdexcept>

#include "common.hpp"
#include "days.hpp"

template <int N,
	  typename T = std::conditional_t<Day<N>::value, Day<N>, void>,
	  typename U = std::conditional_t<Day<N + 1>::value, Day<N + 1>, void>>
struct RunnerImpl;

template <int N>
struct RunnerImpl<N, Day<N>, void> {
    // Base case
    int call(std::optional<int> day, std::optional<int> part) {
	if (!day || *day == N) {
	    return Day<N>().run(part);
	}
	return EXIT_SUCCESS;
    }
    bool match(int n) {
	return n == N;
    }
};

template <int N>
struct RunnerImpl<N, Day<N>, Day<N+1>> {
    using Next = RunnerImpl<N + 1>;
    int call(std::optional<int> day, std::optional<int> part) {
	if (!day || *day == N) {
	    Day<N>().run(part);
	}
	Next().call(day, part);
	return EXIT_SUCCESS;
    }
    bool match(int n) {
	return (n == N) || Next().match(n);
    }
};

template <int N>
struct RunnerImpl<N, void, void> {
    static_assert(false, "This day is not implemented yet");
};

template <int N>
struct RunnerImpl<N, void, Day<N+1>> {
    static_assert(false, "Impossible: Day<N+1> exists but not Day<N>!");

};

using Runner = RunnerImpl<1>;

std::pair<std::optional<int>, std::optional<int>>
get_args(int argc, char *argv[])
{
    std::optional<int> null = {};

    if (argc == 1)
	return std::make_pair(null, null);

    if (argc == 2) {
	int day = std::stoi(argv[1]);
	return std::make_pair(day, null);
    }

    if (argc == 3) {
	int day = std::stoi(argv[1]);
	int part = std::stoi(argv[2]);
	return std::make_pair(day, part);
    }

    throw std::runtime_error("Usage: " + std::string(argv[0]) + " [DAY] [PART]");
}

int main(int argc, char *argv[])
{
    std::pair<std::optional<int>,
	      std::optional<int>> args = get_args(argc, argv);

    std::optional<int> day = args.first;
    std::optional<int> part = args.second;

    Runner runner;

    if (day) {
	if (*day < 1 || *day > 25) {
	    throw std::runtime_error(
		"Invalid day: " + std::to_string(*day));
	}
	if (!runner.match(*day)) {
	    throw std::runtime_error(
		"Day " + std::to_string(*day) + " not implemented");
	}

    }

    if (part) {
	if (*part != 1 && *part != 2) {
	    throw std::runtime_error(
		"Invalid part: " + std::to_string(*part));
	}
    }

    return runner.call(day, part);
}
