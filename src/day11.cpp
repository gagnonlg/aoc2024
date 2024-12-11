#include <list>
#include <map>
#include <numeric>

#include "common.hpp"

namespace day11 {

using InputData = std::list<uint64_t>;

InputData read_input_data(const std::string& input)
{
    std::vector<int64_t> ints = split_ints(input);
    std::list<uint64_t> uints;
    std::transform(ints.begin(), ints.end(), std::back_inserter(uints),
		   [](int64_t i){return static_cast<uint64_t>(i);});
    return uints;
}

// length_blink(x, i, n)
// if i == n: 1
// else: sum(length(x', n - 1) for x' in blink(x))

// length_blink(17, 6)
// 1 7
// 2024 14168
// 20 24 28676032
// 2 0 2 4 2867 6032
// 4048 1 4048 8096 28 67 60 32
// 40 48 2024 40 48 80 96 2 8 6 7 6 0 3 2

// length_blink(17, 0) = 1

// cache[17, 1] = {1,7}
// length_blink(17, 1) = length_blink(1, 0) + length_blink(7, 0) = 2

// length_blink(17, 2) = length_blink(1, 1) + length_blink(7, 1)

// cache[1] = {2024}
  // cache[2024] = {20, 24}
   // cache[20] = {2,0}
    // cache[2] = {4048}
     // cache[4048] = {40}




// cache[17] = {{1,7}}
// cache[1] = {{2024}}
// cache[7] = {{14168}}
// cache[2024] = {{20, 24}}
// cache[14168] = {{28676032}}
// cache[20] = {{2, 0}}
// cache[24] = {{2, 24}}
// cache[28676032] = {{2867,6032}}
// cache[2] = {4048}
// cache[0] = {1}
// --
// cache[4] = {8096}
// cache[

using Cache = std::map<std::pair<uint64_t, uint64_t>, uint64_t>;

void blink(InputData& data, uint64_t n = 1)
{
    for (uint64_t in = 0; in < n; in++) {
	for (auto it = data.begin(); it != data.end(); it++) {
	    if (*it == 0) {
		*it = 1;
		continue;
	    }

	    std::string istr = std::to_string(*it);
	    if (istr.size() % 2 == 0) {
		uint64_t half = istr.size() / 2;
		uint64_t lhs = std::stoull(istr.substr(0, half));
		uint64_t rhs = std::stoull(istr.substr(half, half));
		data.insert(it, lhs);
		*it = rhs;
		continue;
	    }

	    uint64_t a = *it;
	    uint64_t b = 2024;
	    *it = a * b;
	    CHECK(!(a != 0 && *it / a != b)); // Overflow!
	}
    }
}

uint64_t length_blink(Cache& cache, uint64_t x, uint64_t n)
{
    if (n == 0) {
	return 1;
    }

    auto it = cache.find({x, n});
    if (it != cache.end()) {
	return it->second;
    }

    std::list<uint64_t> xs = {x};
    blink(xs);
    cache.insert({{x, 1}, static_cast<uint64_t>(std::distance(xs.begin(), xs.end()))});
    
    uint64_t l = std::accumulate(xs.begin(), xs.end(), 0ull,
				 [&](uint64_t acc, uint64_t y){
				     return acc + length_blink(cache, y, n - 1);
				 });

    cache.insert({{x, n}, l});
    return l;
}

uint64_t length_blink(const InputData& data, uint64_t n)
{
    Cache cache;
    return std::accumulate(data.begin(), data.end(), 0ull,
			   [&](uint64_t acc, uint64_t x){
			       return acc + length_blink(cache, x, n);
			   });
}


Answer part_1(const std::string& input)
{
    InputData data = read_input_data(input);
    return ans(length_blink(data, 25));
}

Answer part_2(const std::string& input)
{
    InputData data = read_input_data(input);
    return ans(length_blink(data, 75));
}

void tests()
{
    std::vector<const char *> test_input = {
	// 0
	"0 1 10 99 999",
	// 1
	"125 17"
    };

    std::vector<InputData> test_data;
    std::transform(test_input.begin(), test_input.end(),
		   std::back_inserter(test_data), read_input_data);

    CHECK(test_data[0] == std::list<uint64_t>({0,1,10,99,999}));

    std::list<uint64_t> copy = test_data[0];
    blink(copy);
    CHECK(copy == std::list<uint64_t>({1,2024,1,0,9,9,2021976}));

    copy = test_data[1];
    CHECK(length_blink(copy, 1) == length_blink(test_data[1], 1));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({253000,1,7}));
    CHECK(length_blink(test_data[1], 1) == 3);
    CHECK(length_blink(copy, 1) == length_blink(test_data[1], 2));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({253,0,2024,14168}));
    CHECK(length_blink(test_data[1], 2) == 4);
    CHECK(length_blink(copy, 1) == length_blink(test_data[1], 3));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({512072,1,20,24,28676032}));
    CHECK(length_blink(test_data[1], 3) == 5);
    CHECK(length_blink(copy, 1) == length_blink(test_data[1], 4));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({512,72,2024,2,0,2,4,2867,6032}));
    CHECK(length_blink(test_data[1], 4) == 9);
    CHECK(length_blink(copy, 1) == length_blink(test_data[1], 5));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({1036288,7,2,20,24,4048,1,4048,8096,28,67,60,32}));
    CHECK(length_blink(test_data[1], 5) == 13);
    CHECK(length_blink(copy, 1) == length_blink(test_data[1], 6));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({2097446912,14168,4048,2,0,2,4,40,48,2024,40,48,80,96,2,8,6,7,6,0,3,2}));
    CHECK(length_blink(test_data[1], 6) == 22);

    copy = test_data[1];
    blink(copy, 25);
    CHECK(std::distance(copy.begin(), copy.end()) == 55312);

    CHECK(part_1(test_input[1]) == 55312);
}

} //namespace day11

