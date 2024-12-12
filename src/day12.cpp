#include <numeric>
#include <set>

#include <boost/pending/disjoint_sets.hpp>

#include "common.hpp"

namespace day12 {

struct Cell {
    uint64_t label;
    char type;
    size_t i;
    size_t j;
};

Cell make_cell(size_t i, size_t j, char c)
{
    return Cell{0, c, i, j};
}

Grid<Cell> read_input_data(const std::string& input)
{
    return Grid<Cell>(input, make_cell);
}

class DisjointSets {
 public:
    using Label = uint64_t;
    explicit DisjointSets(std::size_t initial_size = 128)
	: m_size(initial_size),
	  m_rank(m_size),
	  m_parent(m_size),
	  m_ds(&m_rank[0], &m_parent[0]) {}

    Label makeSet() {
    // Empirically, m_size = 128 seems to be good default. If we
    // exceed this, take a performance hit and do the right thing.
	while (m_globalId >= m_size) {
	    m_size *= 2;
	    m_rank.resize(m_size);
	    m_parent.resize(m_size);
	    m_ds = boost::disjoint_sets<std::size_t*, std::size_t*>(&m_rank[0],
								    &m_parent[0]);
	}
	m_ds.make_set(m_globalId);
	return m_globalId++;
    }

    void unionSet(std::size_t x, std::size_t y) { m_ds.union_set(x, y); }
    Label findSet(std::size_t x) { return m_ds.find_set(x); }

private:
    std::size_t m_globalId = 1;
    std::size_t m_size;
    std::vector<std::size_t> m_rank;
    std::vector<std::size_t> m_parent;
    boost::disjoint_sets<std::size_t*, std::size_t*> m_ds;
};

void label_clusters(Grid<Cell>& grid)
{
    DisjointSets ds{};

    for (size_t i = 0; i < grid.nrows; i++) {
	for (size_t j = 0; j < grid.ncols; j++) {
	    // Allocate label if needed
	    if (grid.at(i, j).label == 0) {
		grid.ref(i, j).label = ds.makeSet();
	    }
	    // Check left
	    if (j > 0 && grid.at(i, j).type == grid.at(i, j - 1).type) {
		CHECK(grid.at(i, j).label != 0);
		CHECK(grid.at(i, j - 1).label != 0);
		ds.unionSet(grid.at(i, j).label, grid.at(i, j - 1).label);
	    }
	    // Check up
	    if (i > 0 && grid.at(i, j).type == grid.at(i - 1, j).type) {
		CHECK(grid.at(i, j).label != 0);
		CHECK(grid.at(i - 1, j).label != 0);
		ds.unionSet(grid.at(i, j).label, grid.at(i - 1, j).label);
	    }

	}
    }

    // Merge labels
    for (size_t i = 0; i < grid.nrows; i++) {
	for (size_t j = 0; j < grid.ncols; j++) {
	    grid.ref(i, j).label = ds.findSet(grid.at(i, j).label);
	}
    }
}

using Region = std::vector<Cell>;
std::vector<Region> make_regions(Grid<Cell> grid)
{
    if (grid.m_buf.empty()) {
	return {};
    }

    std::sort(grid.m_buf.begin(), grid.m_buf.end(),
	      [](auto lhs, auto rhs){return lhs.label < rhs.label;});


    Region r;
    std::vector<Region> rs;
    uint64_t l = grid.m_buf.front().label;
    for (auto& cell : grid.m_buf) {
	if (cell.label != l) {
	    rs.push_back(std::move(r));
	    r = Region();
	    l = cell.label;
	}
	r.push_back(std::move(cell));
    }
    rs.push_back(std::move(r));
    return rs;
}

std::vector<Region> get_regions(const std::string& input)
{
    Grid<Cell> grid = read_input_data(input);
    label_clusters(grid);
    return make_regions(std::move(grid));
}

uint64_t calculate_perimeter(const Region& r)
{
    std::set<std::pair<int64_t, int64_t>> set;
    for (const Cell& c : r) {
	set.insert({ans(c.i), ans(c.j)});
    }
    uint64_t p = 0;
    for (auto& [i, j] : set) {
	if (!set.contains({i - 1, j}))
	    p++;
	if (!set.contains({i, j + 1}))
	    p++;
	if (!set.contains({i + 1, j}))
	    p++;
	if (!set.contains({i, j - 1}))
	    p++;
    }
    return p;
}

uint64_t calculate_price(const Region& r)
{
    uint64_t area = r.size();
    uint64_t perimeter = calculate_perimeter(r);
    // std::cout << "Type:" << r.front().type << " Area:" << area << " Perimeter:" << perimeter << " Price:" << (area*perimeter) << std::endl;
    return area * perimeter;
}

uint64_t calculate_price(const std::vector<Region>& rs)
{
    return std::accumulate(rs.begin(), rs.end(), 0ull,
			   [](uint64_t acc, const Region& r){
			       return acc + calculate_price(r);
			   });
}

Answer part_1(const std::string& input)
{
    return ans(calculate_price(get_regions(input)));
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
}

void tests()
{
    std::vector<const char *> test_input = {
	// 0
	"AAAA\n"
	"BBCD\n"
	"BBCC\n"
	"EEEC\n",
	// 1
	"OOOOO\n"
	"OXOXO\n"
	"OOOOO\n"
	"OXOXO\n"
	"OOOOO\n",
	// 2
	"RRRRIICCFF\n"
	"RRRRIICCCF\n"
	"VVRRRCCFFF\n"
	"VVRCCCJFFF\n"
	"VVVVCJJCFE\n"
	"VVIVCCJJEE\n"
	"VVIIICJJEE\n"
	"MIIIIIJJEE\n"
	"MIIISIJEEE\n"
	"MMMISSJEEE\n",
    };

    std::vector<Grid<Cell>> grids;
    std::transform(test_input.begin(),
		   test_input.end(),
		   std::back_inserter(grids),
		   read_input_data);

    CHECK(grids.size() == test_input.size());
    CHECK(grids[0].nrows == 4);
    CHECK(grids[0].ncols == 4);
    CHECK(grids[0].at(0, 0).label == 0);
    CHECK(grids[0].at(0, 0).type == 'A');
    CHECK(grids[0].at(0, 0).i == 0);
    CHECK(grids[0].at(0, 0).j == 0);

    label_clusters(grids[0]);
    std::vector<Region> regions = make_regions(grids[0]);
    CHECK(regions.size() == 5);

    std::vector<std::vector<Region>> test_regions;
    std::transform(test_input.begin(),
		   test_input.end(),
		   std::back_inserter(test_regions),
		   get_regions);

    CHECK(test_regions.size() == test_input.size());
    CHECK(test_regions.at(0).size() == 5);
    CHECK(test_regions.at(1).size() == 5);
    CHECK(test_regions.at(2).size() == 11);

    std::vector<uint64_t> test_prices;
    std::transform(test_regions.begin(),
		   test_regions.end(),
		   std::back_inserter(test_prices),
		   [](const std::vector<Region>& rs){return calculate_price(rs);});
    CHECK(test_prices.at(0) == 140);
    CHECK(test_prices.at(1) == 772);
    CHECK(test_prices.at(2) == 1930);

    CHECK(part_1(test_input.at(0)) == 140);
    CHECK(part_1(test_input.at(1)) == 772);
    CHECK(part_1(test_input.at(2)) == 1930);
		   
    
}

} //namespace day12

