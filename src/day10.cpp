#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include "common.hpp"

namespace day10 {




struct Node {
    char value;
    std::optional<std::unordered_set<size_t>> score;
};

struct NodeGrid {
    std::vector<Node> buf;
    size_t nrows;
    size_t ncols;
    size_t idx(size_t i, size_t j) const {
	CHECK(i < nrows);
	CHECK(j < ncols);
	return i * nrows + j;
    }
    Node& at(size_t i) {
	return buf.at(i);
    }
    Node& at(size_t i, size_t j) {
	return at(idx(i, j));
    }
    const Node& at(size_t i, size_t j) const {
	return buf.at(idx(i, j));
    }
};

NodeGrid read_nodes(const std::string& input)
{
    NodeGrid grid;
    std::vector<std::string> lines = split_lines(input);
    CHECK(!lines.empty());
    grid.nrows = lines.size();
    grid.ncols = lines.front().size();
    for (size_t i = 0; i < grid.nrows; i++) {
	CHECK(lines.at(i).size() == grid.ncols);
	for (size_t j = 0; j < grid.ncols; j++) {
	    grid.buf.push_back({lines.at(i).at(j), {}});
	}
    }
    return grid;
}

struct NodeMap {
    std::unordered_map<size_t, std::vector<size_t>> map;
    std::vector<size_t> roots;
};


// Recrod segment from (i, j) if it's one from (i_, j_)
bool valid_segment(const NodeGrid& grid,
		   size_t i, size_t j,
		   size_t i_, size_t j_)
{
    if (i >= grid.nrows || i_ >= grid.nrows ||
	j >= grid.ncols || j_ >= grid.ncols)
	return false;

    char c0 = grid.at(i, j).value;
    char c1 = grid.at(i_, j_).value;

    if (c1 > c0)
	return ((c1 - c0) == 1);
    else
	return ((c0 - c1) == 1);
}

NodeMap make_map(const NodeGrid& grid)
{
    NodeMap map;
    for (size_t i = 0; i < grid.nrows; i++) {
	for (size_t j = 0; j < grid.ncols; j++) {
	    char v = grid.at(i, j).value;
	    if (v == '0') {
		map.roots.push_back(grid.idx(i, j));
	    }
	    // Right neighbor
	    if (valid_segment(grid, i, j, i, j + 1)) {
		char v2 = grid.at(i, j + 1).value;
		if (v2 > v)
		    map.map[grid.idx(i, j)].push_back(grid.idx(i, j + 1));
		else
		    map.map[grid.idx(i, j + 1)].push_back(grid.idx(i, j));
	    }
	    // Down neighbor
	    if (valid_segment(grid, i, j, i + 1, j)) {
		char v2 = grid.at(i + 1, j).value;
		if (v2 > v)
		    map.map[grid.idx(i, j)].push_back(grid.idx(i + 1, j));
		else
		    map.map[grid.idx(i + 1, j)].push_back(grid.idx(i, j));
	    }
	}
    }
    return map;
}

void find_nines(size_t i, NodeMap& map, NodeGrid& grid)
{
    if (grid.at(i).score.has_value())
	return;

    grid.at(i).score = std::unordered_set<size_t>();

    if (grid.at(i).value == '9') {
	grid.at(i).score->insert(i);
	return;
    }

    auto it = map.map.find(i);
    if (it != map.map.end()) {
	for (size_t j : it->second) {
	    find_nines(j, map, grid);
	    grid.at(i).score->insert(grid.at(j).score->begin(),
				     grid.at(j).score->end());
	}
    }
}

int64_t trail_score(size_t i, NodeMap& map, NodeGrid& grid)
{
    CHECK(grid.at(i).value == '0');
    find_nines(i, map, grid);
    CHECK(grid.at(i).score.has_value());
    return ans(grid.at(i).score->size());
}

int64_t total_score(NodeMap& map, NodeGrid& grid)
{
    return std::accumulate(map.roots.begin(), map.roots.end(),
			   0ll, [&](int64_t acc, size_t i) {
			       return acc + trail_score(i, map, grid);
			   });
}

Answer part_1(const std::string& input)
{
    NodeGrid grid = read_nodes(input);
    NodeMap map = make_map(grid);
    return total_score(map, grid);
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
}

void tests()
{
    std::vector<const char *> test_input = {
	// 0
	"0123\n"
	"1234\n"
	"8765\n"
	"9876\n",
	// 1
	"...0...\n"
	"...1...\n"
	"...2...\n"
	"6543456\n"
	"7.....7\n"
	"8.....8\n"
	"9.....9\n",
	// 2
	"..90..9\n"
	"...1.98\n"
	"...2..7\n"
	"6543456\n"
	"765.987\n"
	"876....\n"
	"987....\n",
	// 3
	"10..9..\n"
	"2...8..\n"
	"3...7..\n"
	"4567654\n"
	"...8..3\n"
	"...9..2\n"
	".....01\n",
	// 4
	"89010123\n"
	"78121874\n"
	"87430965\n"
	"96549874\n"
	"45678903\n"
	"32019012\n"
	"01329801\n"
	"10456732\n"
    };

    std::vector<NodeGrid> grids;
    std::transform(test_input.begin(), test_input.end(), std::back_inserter(grids),
		   read_nodes);

    CHECK(grids.at(0).nrows == 4);
    CHECK(grids.at(0).ncols == 4);
    CHECK(grids.at(0).at(0, 0).value == '0');
    CHECK(!grids.at(0).at(0, 0).score);
    CHECK(grids.at(0).at(3, 3).value == '6');
    CHECK(!grids.at(0).at(3, 3).score);

    CHECK(valid_segment(grids[0], 0, 0, 0, 1));
    CHECK(valid_segment(grids[0], 0, 0, 1, 0));
    CHECK(!valid_segment(grids[0], 1, 0, 2, 0));

    std::vector<NodeMap> maps;
    std::transform(grids.begin(), grids.end(), std::back_inserter(maps), make_map);
    CHECK(maps.size() == grids.size());
    CHECK(maps[0].roots.size() == 1);
    CHECK(maps[0].roots.front() == grids[0].idx(0, 0));
    std::vector<size_t> v;
    v = maps[0].map.at(grids[0].idx(0, 0));
    CHECK(v.size() == 2);
    CHECK(std::find(v.begin(), v.end(), grids[0].idx(0, 1)) != v.end());
    CHECK(std::find(v.begin(), v.end(), grids[0].idx(1, 0)) != v.end());

    // mark_reaching_end(maps[0], grids[0]);
    // CHECK(grids[0].at(0, 0).score == 1);
    // mark_reaching_end(maps[1], grids[1]);
    // CHECK(grids[1].at(0, 0).score == 1);

    CHECK(trail_score(0, maps[0], grids[0]) == 1);
    CHECK(trail_score(3, maps[1], grids[1]) == 2);
    CHECK(trail_score(3, maps[2], grids[2]) == 4);
    CHECK(trail_score(1, maps[3], grids[3]) == 1);
    CHECK(trail_score(grids[3].idx(6,5), maps[3], grids[3]) == 2);

    CHECK(total_score(maps[4], grids[4]) == 36);

    CHECK(part_1(test_input[4]) == 36);
    
}

} //namespace day10

