#include <numeric>

#include "common.hpp"

namespace day1 {

std::vector<int64_t> compute_distances(const std::vector<int64_t>& lhs,
				       const std::vector<int64_t>& rhs)
{
    CHECK(lhs.size() == rhs.size());
    std::vector<int64_t> dists;
    for (size_t i = 0; i < lhs.size(); i++)
	dists.push_back(std::abs(lhs[i] - rhs[i]));
    return dists;
}

Answer part_1(const std::string& input)
{
    std::vector<std::vector<int64_t>> columns = split_int_columns(input);
    CHECK(columns.size() == 2);
    sort(columns[0].begin(), columns[0].end());
    sort(columns[1].begin(), columns[1].end());
    std::vector<int64_t> dists = compute_distances(columns[0], columns[1]);
    return std::accumulate(dists.begin(), dists.end(), 0);
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
}

void tests()
{
    const char *test_input_1 =
	"3   4\n"
	"4   3\n"
	"2   5\n"
	"1   3\n"
	"3   9\n"
	"3   3\n";

    std::vector<std::vector<int64_t>> columns = split_int_columns(test_input_1);
    CHECK(columns.size() == 2);
    CHECK(columns[0] == std::vector<int64_t>({3,4,2,1,3,3}));
    CHECK(columns[1] == std::vector<int64_t>({4,3,5,3,9,3}));

    sort(columns[0].begin(), columns[0].end());
    sort(columns[1].begin(), columns[1].end());
    std::vector<int64_t> dists = compute_distances(columns[0], columns[1]);
    CHECK(dists == std::vector<int64_t>({2,1,0,1,2,5}));
    CHECK(part_1(test_input_1) == 11);
}

} //namespace day1

