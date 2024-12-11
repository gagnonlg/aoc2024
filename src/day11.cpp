#include <list>

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

Answer part_1(const std::string& input)
{
    InputData data = read_input_data(input);
    blink(data, 25);
    return std::distance(data.begin(), data.end());
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
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
    blink(copy);
    CHECK(copy == std::list<uint64_t>({253000,1,7}));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({253,0,2024,14168}));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({512072,1,20,24,28676032}));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({512,72,2024,2,0,2,4,2867,6032}));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({1036288,7,2,20,24,4048,1,4048,8096,28,67,60,32}));
    blink(copy);
    CHECK(copy == std::list<uint64_t>({2097446912,14168,4048,2,0,2,4,40,48,2024,40,48,80,96,2,8,6,7,6,0,3,2}));

    copy = test_data[1];
    blink(copy, 25);
    CHECK(std::distance(copy.begin(), copy.end()) == 55312);

    CHECK(part_1(test_input[1]) == 55312);
}

} //namespace day11

