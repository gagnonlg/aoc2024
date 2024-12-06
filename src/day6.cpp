#include <set>
#include <unordered_set>

#include "common.hpp"

namespace day6 {

struct InputData {
    std::set<std::pair<int64_t,int64_t>> obstacles;
    std::set<std::pair<int64_t,int64_t>> visited;
    std::pair<int64_t, int64_t> startpos;
    std::pair<int64_t, int64_t> bounds;
    
    void add_obstacle(size_t i, size_t j) {
	obstacles.insert(std::make_pair(ans(i), ans(j)));
    }

    int64_t count_visited(int64_t i, int64_t j) {
	return visited.insert(std::make_pair(i, j)).second ? 1 : 0;
    }

    bool obstacle_at(int64_t i, int64_t j) const {
	return obstacles.contains({i, j});
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

std::vector<std::pair<int64_t, int64_t>> trace_path(const InputData& data)
{
    std::vector<std::pair<int64_t, int64_t>> trace;

    auto [i, j] = data.startpos;
    int64_t i_dir = -1;
    int64_t j_dir = 0;
    
    while (!data.out_of_bounds(i, j)) {
	trace.emplace_back(i, j);

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

    return trace;
}

bool has_cycle_with_obs(const InputData& data, int64_t i_obs, int64_t j_obs)
{
    auto [i, j] = data.startpos;
    int64_t i_dir = -1;
    int64_t j_dir = 0;

    if (i == i_obs && j == j_obs) {
	return false;
    }

    std::set<std::tuple<int64_t,int64_t,int64_t,int64_t>> seen;
    
    while (!data.out_of_bounds(i, j)) {
	// Have I already seen this one?
	if (!seen.insert({i, j, i_dir, j_dir}).second) {
	    // Le cycle!
	    return true;
	}
	int64_t i_next = i + i_dir;
	int64_t j_next = j + j_dir;
	while (data.obstacle_at(i_next, j_next) ||
	       (i_next == i_obs && j_next == j_obs)) {
	    rotate_right(i_dir, j_dir);
	    i_next = i + i_dir;
	    j_next = j + j_dir;
	}
	i = i_next;
	j = j_next;
    }

    return false;
}


Answer part_1(const std::string& input)
{
    InputData data = read_input_data(input);
    std::vector<std::pair<int64_t, int64_t>> trace = trace_path(data);
    std::sort(trace.begin(), trace.end());
    return std::unique(trace.begin(), trace.end()) - trace.begin();
}

Answer part_2(const std::string& input)
{
    InputData data = read_input_data(input);
    std::vector<std::pair<int64_t, int64_t>> trace = trace_path(data);
    std::sort(trace.begin(), trace.end());
    auto it = std::unique(trace.begin(), trace.end());
    trace.resize(static_cast<size_t>(std::distance(trace.begin(), it)));
    return std::count_if(trace.begin(), trace.end(), [&](const auto& pos){
	return has_cycle_with_obs(data, pos.first, pos.second);
    });
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

    /////////////////////////////////////////////////////
     
#define CHECK_CYCLE(i, j) CHECK(has_cycle_with_obs(data, i, j))  
    CHECK_CYCLE(6, 3);
    CHECK_CYCLE(7, 6);
    CHECK_CYCLE(7, 7);
    CHECK_CYCLE(8, 1);
    CHECK_CYCLE(8, 3);
    CHECK_CYCLE(9, 7);

    CHECK(part_2(test_input_1) == 6);

    
}

} //namespace day6

