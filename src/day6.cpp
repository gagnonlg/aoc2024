#include <unordered_set>

#include "common.hpp"

namespace day6 {

struct InputData {
    std::unordered_set<int64_t> obstacles;
    std::unordered_set<int64_t> visited;
    std::pair<int64_t, int64_t> startpos;
    std::pair<int64_t, int64_t> bounds;
    
    void add_obstacle(size_t i, size_t j) {
	obstacles.insert(ans(i) * bounds.second + ans(j));
    }

    int64_t count_visited(int64_t i, int64_t j) {
	return visited.insert(i * bounds.second + j).second ? 1 : 0;
    }

    bool obstacle_at(int64_t i, int64_t j) const {
	return obstacles.contains(i * bounds.second + j);
    }

    bool out_of_bounds(int64_t i, int64_t j) const {
	return i < 0 || i >= bounds.first || j < 0 || j >= bounds.second;
    }
};

InputData read_input_data(const std::string& str)
{
    InputData data;
    std::vector<std::string> lines = split_lines(str);
    CHECK(!lines.empty());
    data.bounds = std::make_pair(ans(lines.size()), ans(lines[0].size()));
    for (size_t i = 0; i < lines.size(); i++) {
	CHECK(ans(lines[i].size()) == data.bounds.second);
	for (size_t j = 0; j < lines[i].size(); j++) {
	    switch (lines[i][j]) {
	    case '^':
		data.startpos = std::make_pair(i, j);
		continue;
	    case '#':
		data.add_obstacle(i, j);
		continue;
	    }
	}
    }
    return data;
}
    
void rotate_right(int64_t& i, int64_t& j)
{
    int64_t tmp = i;
    i = j;
    j = -tmp;
}

Answer part_1(const std::string& input)
{
    InputData data = read_input_data(input);
    auto [i, j] = data.startpos;

    int64_t i_dir = -1;
    int64_t j_dir = 0;
    
    Answer dist = 0;
    while (!data.out_of_bounds(i, j)) {
	dist += data.count_visited(i, j);

	int64_t i_next = i + i_dir;
	int64_t j_next = j + j_dir;
	while (data.obstacle_at(i_next, j_next)) {
	    rotate_right(i_dir, j_dir);
	    i_next = i + i_dir;
	    j_next = j + j_dir;
	}
	i = i_next;
	j = j_next;
    }

    return dist;
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
}

void tests()
{
    const char * test_input_1 =
	"....#.....\n"
	".........#\n"
	"..........\n"
	"..#.......\n"
	".......#..\n"
	"..........\n"
	".#..^.....\n"
	"........#.\n"
	"#.........\n"
	"......#...\n";

    InputData data = read_input_data(test_input_1);
    CHECK(data.obstacles.size() == 8);
    CHECK(data.obstacle_at(0, 4));
    CHECK(!data.obstacle_at(1,5));
    CHECK(data.out_of_bounds(-1, 2));
    CHECK(data.out_of_bounds(2, -1));
    CHECK(data.out_of_bounds(0, 10));
    CHECK(data.out_of_bounds(10, 0));
    CHECK(!data.out_of_bounds(5,5));
    CHECK(data.startpos.first == 6);
    CHECK(data.startpos.second == 4);

    int64_t i = 0;
    int64_t j = 1;
    rotate_right(i, j);
    CHECK(i == 1 && j == 0);
    rotate_right(i, j);
    CHECK(i == 0 && j == -1);
    rotate_right(i, j);
    CHECK(i == -1 && j == 0);
    rotate_right(i, j);
    CHECK(i == 0 && j == 1);

    CHECK(part_1(test_input_1) == 41);

}

} //namespace day6

