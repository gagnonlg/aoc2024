#include <map>
#include <set>
#include "common.hpp"

namespace day8 {

struct InputData {
    std::map<std::pair<int64_t, int64_t>, char> map;
    std::pair<int64_t, int64_t> bounds;
};


InputData read_input_data(const std::string& input)
{
    std::vector<std::string> lines = split_lines(input);
    CHECK(!lines.empty());

    InputData data;
    data.bounds = std::make_pair(lines.size(), lines[0].size());

    for (size_t i = 0; i < lines.size(); i++) {
	for (size_t j = 0; j < lines[i].size(); j++) {
	    if (lines[i][j] != '.' && lines[i][j] != '#')
		data.map.insert({{i, j}, lines[i][j]});
	}
    }
    return data;
}

std::set<std::pair<int64_t, int64_t>> read_solution(const std::string& input)
{
    std::vector<std::string> lines = split_lines(input);
    std::set<std::pair<int64_t, int64_t>> solution;
    for (size_t i = 0; i < lines.size(); i++) {
	for (size_t j = 0; j < lines[i].size(); j++) {
	    if (lines[i][j] == '#')
		solution.insert({i, j});
	}
    }
    return solution;
}


std::pair<int64_t, int64_t> compute_diff(const std::pair<int64_t, int64_t>& x,
					 const std::pair<int64_t, int64_t>& y)
{
    int64_t di = y.first - x.first;
    int64_t dj = y.second - x.second;
    return std::make_pair(di, dj);
}

bool in_bounds(const InputData& data, const std::pair<int64_t, int64_t>& x)
{
    return x.first >= 0 && x.first < data.bounds.first &&
	x.second >= 0 && x.second < data.bounds.second;
}

std::set<std::pair<int64_t, int64_t>>
lookup_antinodes(const InputData& data,
		 const std::pair<int64_t, int64_t>& x,
		 const std::pair<int64_t, int64_t>& y)
{
    CHECK(x < y);
    auto it_x = data.map.find(x);
    auto it_y = data.map.find(y);
    CHECK(it_x != data.map.end());
    CHECK(it_y != data.map.end());
    CHECK(it_x->second == it_y->second);
    
    std::set<std::pair<int64_t, int64_t>> antinodes;
    std::pair<int64_t, int64_t> dist = compute_diff(x, y);
    std::pair<int64_t, int64_t> anti1 = std::make_pair(x.first + 2 * dist.first,
						       x.second + 2 * dist.second);
    std::pair<int64_t, int64_t> anti2 = std::make_pair(y.first - 2 * dist.first,
						       y.second - 2 * dist.second);

    if (in_bounds(data, anti1) &&
	(data.map.find(anti1) == data.map.end()
	 || data.map.find(anti1)->second != it_x->second))
	    antinodes.insert(std::move(anti1));

    if (in_bounds(data, anti2) &&
	(data.map.find(anti2) == data.map.end()
	 || data.map.find(anti2)->second != it_x->second))
	    antinodes.insert(std::move(anti2));

    return antinodes;
}

std::set<std::pair<int64_t, int64_t>>
lookup_doublets_backwards(const InputData& data,
			  const std::pair<int64_t, int64_t>& x)
{
    auto le_x = data.map.find(x);
    CHECK(le_x != data.map.end());
    char c = le_x->second;
    std::set<std::pair<int64_t, int64_t>> starts;
    for (auto it = data.map.begin(); it != le_x; it++) {
	if (it->second == c)
	    starts.insert(it->first);
    }
    return starts;
}

std::set<std::pair<int64_t, int64_t>>
lookup_antinodes(const InputData& data,
		 const std::pair<int64_t, int64_t>& x)
{
    std::set<std::pair<int64_t, int64_t>> antinodes;
    std::set<std::pair<int64_t, int64_t>> doublets =
	lookup_doublets_backwards(data, x);
    for (const auto& start : doublets) {
	std::set<std::pair<int64_t, int64_t>> as
	    = lookup_antinodes(data, start, x);
	antinodes.insert(as.begin(), as.end());
    }
    return antinodes;
}

std::set<std::pair<int64_t, int64_t>>
lookup_antinodes(const InputData& data)
{
    std::set<std::pair<int64_t, int64_t>> antinodes;
    for (const auto& x : data.map) {
	std::set<std::pair<int64_t, int64_t>> as
	    = lookup_antinodes(data, x.first);
	antinodes.insert(as.begin(), as.end());
    }
    return antinodes;
}

Answer part_1(const std::string& input)
{
    InputData data = read_input_data(input);
    return ans(lookup_antinodes(data).size());
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
}

void tests()
{
    std::vector<const char *> test_input = {
	// 0
	"............\n"
	"........0...\n"
	".....0......\n"
	".......0....\n"
	"....0.......\n"
	"......A.....\n"
	"............\n"
	"............\n"
	"........A...\n"
	".........A..\n"
	"............\n"
	"............\n",
	// 1
	"..........\n"
	"...#......\n"
	"..........\n"
	"....a.....\n"
	"..........\n"
	".....a....\n"
	"..........\n"
	"......#...\n"
	"..........\n"
	"..........\n",
	// 2
	"..........\n"
	"...#......\n"
	"#.........\n"
	"....a.....\n"
	"........a.\n"
	".....a....\n"
	"..#.......\n"
	"......#...\n"
	"..........\n"
	"..........\n",
	// 3
	"..........\n"
	"...#......\n"
	"#.........\n"
	"....a.....\n"
	"........a.\n"
	".....a....\n"
	"..#.......\n"
	"......A...\n"
	"..........\n"
	"..........\n",
	// 4
	"......#....#\n"
	"...#....0...\n"
	"....#0....#.\n"
	"..#....0....\n"
	"....0....#..\n"
	".#....A.....\n"
	"...#........\n"
	"#......#....\n"
	"........A...\n"
	".........A..\n"
	"..........#.\n"
	"..........#.\n"
    };

    std::vector<InputData> test_data;
    std::transform(test_input.begin(), test_input.end(),
		   std::back_inserter(test_data), read_input_data);

    CHECK(test_data.size() == 5);
    CHECK(test_data[0].map.size() == 7);
    CHECK(test_data[0].bounds.first == 12);
    CHECK(test_data[0].bounds.second == 12);

#define CHECK_HAS_CHAR(i, ii, jj, c) do {		\
	auto it = test_data[i].map.find({ii, jj});	\
	CHECK(it != test_data[i].map.end());	\
	CHECK(it->second == c);			\
    } while (0)

    CHECK_HAS_CHAR(0, 1, 8, '0');
    CHECK_HAS_CHAR(0, 2, 5, '0');
    CHECK_HAS_CHAR(0, 3, 7, '0');
    CHECK_HAS_CHAR(0, 4, 4, '0');
    CHECK_HAS_CHAR(0, 5, 6, 'A');
    CHECK_HAS_CHAR(0, 8, 8, 'A');
    CHECK_HAS_CHAR(0, 9, 9, 'A');

    CHECK(compute_diff({3,4}, {5,5}).first == 2);
    CHECK(compute_diff({3,4}, {5,5}).second == 1);

    CHECK(in_bounds(test_data[0], {1,2}));
    CHECK(!in_bounds(test_data[0], {-1,2}));
    CHECK(!in_bounds(test_data[0], {1,-2}));
    CHECK(!in_bounds(test_data[0], {1,12}));
    CHECK(!in_bounds(test_data[0], {12,1}));

    auto a = lookup_antinodes(test_data[2], {3, 4}, {5, 5});
    CHECK(a.size() == 2);
    CHECK(a.contains({1,3}));
    CHECK(a.contains({7,6}));
    a = lookup_antinodes(test_data[2], {3, 4}, {4, 8});
    CHECK(a.size() == 1);
    CHECK(a.contains({2,0}));
    a = lookup_antinodes(test_data[2], {4, 8}, {5, 5});
    CHECK(a.size() == 1);
    CHECK(a.contains({6,2}));

    a = lookup_doublets_backwards(test_data[3], {3,4});
    CHECK(a.empty());
    a = lookup_doublets_backwards(test_data[3], {4,8});
    CHECK(a.size() == 1);
    CHECK(a.contains({3,4}));
    a = lookup_doublets_backwards(test_data[3], {5,5});
    CHECK(a.size() == 2);
    CHECK(a.contains({3,4}));
    CHECK(a.contains({4,8}));
    a = lookup_doublets_backwards(test_data[3], {7,6});
    CHECK(a.empty());

    a = lookup_antinodes(test_data[3], {3, 4});
    CHECK(a.empty());
    a = lookup_antinodes(test_data[3], {4, 8});
    CHECK(a.size() == 1);
    CHECK(a.contains({2, 0}));
    a = lookup_antinodes(test_data[3], {5, 5});
    CHECK(a.size() == 3);
    CHECK(a.contains({6, 2}));
    CHECK(a.contains({1, 3}));
    CHECK(a.contains({7, 6}));
    a = lookup_antinodes(test_data[3], {7, 6});
    CHECK(a.empty());


    std::set<std::pair<int64_t, int64_t>> s;
#define CHECK_SOLUTION(A,S) do {		\
	CHECK(A.size() >= S.size());		\
	for (const auto& x: S)			\
	    CHECK(A.contains(x));		\
    } while (0)

    a = lookup_antinodes(test_data[1]);
    s = read_solution(test_input[1]);
    CHECK_SOLUTION(a, s);
    a = lookup_antinodes(test_data[2]);
    s = read_solution(test_input[2]);
    CHECK_SOLUTION(a, s);
    a = lookup_antinodes(test_data[3]);
    s = read_solution(test_input[3]);
    CHECK(a.size() == s.size() + 1);
    CHECK_SOLUTION(a, s);
    CHECK(a.contains({7,6}));
    a = lookup_antinodes(test_data[4]);
    s = read_solution(test_input[4]);
    CHECK(a.size() == s.size() + 1);
    CHECK_SOLUTION(a, s);
    CHECK(a.contains({5,6}));
    
    CHECK(part_1(test_input[0]) == 14);
    
}

} //namespace day8

