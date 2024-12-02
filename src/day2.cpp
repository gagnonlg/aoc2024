#include "common.hpp"

namespace day2 {

std::vector<int64_t> compute_diffs(const std::vector<int64_t>& nums)
{
    std::vector<int64_t> diffs;
    for (size_t i = 1; i < nums.size(); i++) {
	diffs.push_back(nums[i] - nums[i - 1]);
    }
    return diffs;
}

enum Change {
    Increase, Decrease, None
};

Change change(int64_t x)
{
    if (x > 0)
	return Increase;
    if (x < 0)
	return Decrease;
    return None;
}

bool is_report_safe(const std::vector<int64_t>& nums)
{
    std::vector<int64_t> diffs = compute_diffs(nums);
    CHECK(!diffs.empty());
    return std::all_of(diffs.begin(), diffs.end(), [&](int64_t x) {
	return (change(x) == change(diffs[0])) &&
	    (std::abs(x) >= 1) &&
	    (std::abs(x) <= 3);
    });
}

Answer part_1(const std::string& input)
{
    std::vector<std::vector<int64_t>> num_lines = split_int_lines(input);
    return std::count_if(num_lines.begin(), num_lines.end(), is_report_safe);
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
}

void tests()
{
    const char *test_input_1 =
	"7 6 4 2 1\n"
	"1 2 7 8 9\n"
	"9 7 6 2 1\n"
	"1 3 2 4 5\n"
	"8 6 4 4 1\n"
	"1 3 6 7 9\n";

    std::vector<std::vector<int64_t>> num_lines = split_int_lines(test_input_1);
    CHECK(num_lines.size() == 6);
    CHECK(num_lines[0] == std::vector<int64_t>({7,6,4,2,1}));
    CHECK(compute_diffs(num_lines[0]) == std::vector<int64_t>({-1,-2,-2,-1}));
    CHECK(compute_diffs(num_lines[4]) == std::vector<int64_t>({-2,-2,0,-3}));
    CHECK(is_report_safe(num_lines[0]));
    CHECK(!is_report_safe(num_lines[1]));
    CHECK(!is_report_safe(num_lines[2]));
    CHECK(!is_report_safe(num_lines[3]));
    CHECK(!is_report_safe(num_lines[4]));
    CHECK(is_report_safe(num_lines[5]));
    CHECK(part_1(test_input_1) == 2);
}

} //namespace day2

