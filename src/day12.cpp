#include <numeric>
#include <set>
#include <map>


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

bool delta_between(const std::set<std::pair<int64_t, int64_t>>& set,
				   bool prev,
				   size_t i,
				   size_t j)
{
    return set.contains({ans(i), ans(j)}) != prev;
}

using Delta = std::optional<std::pair<char, char>>;

bool operator==(const Delta& lhs, const Delta& rhs)
{
    return lhs && rhs && lhs->first == rhs->first  && lhs->second == rhs->second;
}

std::vector<Delta> row_deltas(const std::map<std::pair<int64_t, int64_t>, char>& map,
			      size_t i_row,
			      size_t j_min,
			      size_t j_max)
{
    std::vector<Delta> ds;
    char prev = '\0';
    bool had_prev = false;
    for (size_t j = j_min; j <= j_max; j++) {
	auto it = map.find({i_row, j});
	if (it != map.end()) {
	    if (had_prev && it->second == prev)
		ds.push_back({});
	    else {
		Delta d = std::make_pair(prev, it->second);
		ds.push_back(d);
		prev = it->second;
	    }
	    had_prev = true;
	} else {
	    if (had_prev) {
		ds.push_back(std::make_pair(prev, '\0'));
	    } else {
		ds.push_back({});
	    }
	    prev = '\0';
	    had_prev = false;
	}
    }
    return ds;
}

std::vector<Delta> col_deltas(const std::map<std::pair<int64_t, int64_t>, char>& map,
			      size_t j_row,
			      size_t i_min,
			      size_t i_max)
{
    std::vector<Delta> ds;
    char prev = '\0';
    bool had_prev = false;
    for (size_t i = i_min; i <= i_max; i++) {
	auto it = map.find({i, j_row});
	if (it != map.end()) {
	    if (had_prev && it->second == prev)
		ds.push_back({});
	    else {
		ds.push_back(Delta({prev, it->second}));
		prev = it->second;
	    }
	    had_prev = true;
	} else {
	    if (had_prev) {
		ds.push_back(std::make_pair(prev, '\0'));
	    } else {
		ds.push_back({});
	    }
	    prev = '\0';
	    had_prev = false;
	}
    }
    return ds;
}

// std::vector<size_t> col_deltas(const std::set<std::pair<int64_t, int64_t>>& set,
// 				size_t j_row,
// 				size_t i_min,
// 				size_t i_max)
// {
//     // std::cout << "COL:" << std::endl;
//     std::vector<size_t> ds;
//     bool prev = false;
//     for (size_t i = i_min; i <= i_max; i++) {
// 	bool d = delta_between(set, prev, i, j_row);
// 	if (d) {
// 	    ds.push_back(1);
// 	    prev = !prev;
// 	} else {
// 	    ds.push_back(0);
// 	}
// 	// std::cout << ds.back() << std::endl;
//     }
//     return ds;
// }

std::vector<Delta> filter_deltas(const std::vector<Delta>& prev,
				 const std::vector<Delta>& cur)
{
    CHECK(prev.size() == cur.size());
    std::vector<Delta> newv;
    for (size_t i = 0; i < prev.size(); i++) {
	newv.push_back((cur[i] && cur[i] == prev[i]) ? Delta({}) : cur[i]);
    }
    return newv;
}

uint64_t count_deltas(const std::vector<std::vector<Delta>>& deltas)
{
    size_t acc = 0;
    for (const std::vector<Delta>& ds : deltas)
	acc += std::accumulate(ds.begin(), ds.end(), 0ull,
			       [](size_t acc_, const Delta& d) {
				   if (d)
				       return acc_ + 1;
				   else
				       return acc_;
			       });
    // std::cout << "COUNT=="<<acc<<std::endl;
    return acc;
}

uint64_t count_sides(const Region& r)
{
    size_t imin = std::numeric_limits<size_t>::max();
    size_t imax = std::numeric_limits<size_t>::min();
    size_t jmin = std::numeric_limits<size_t>::max();
    size_t jmax = std::numeric_limits<size_t>::min();
    std::map<std::pair<int64_t, int64_t>, char> map;
    for (const Cell& c : r) {
	map.insert({{ans(c.i), ans(c.j)}, c.type});
	if (c.i < imin)
	    imin = c.i;
	if (c.i > imax)
	    imax = c.i;
	if (c.j < jmin)
	    jmin = c.j;
	if (c.j > jmax)
	    jmax = c.j;
    }
    // std::cout << "BOX: i=" << imin <<","<<imax<< " j=" << jmin <<","<<jmax<<std::endl;
    
    // Do rows
    std::vector<std::vector<Delta>> j_deltas;
    std::vector<std::vector<Delta>> j_deltas_f;
    for (size_t i = imin; i <= imax; i++) {
	j_deltas.push_back(row_deltas(map, i, jmin, jmax + 1));
	if (i == imin) {
	    j_deltas_f.push_back(j_deltas.at(i - imin));
	}  else {
	    j_deltas_f.push_back(
		filter_deltas(j_deltas.at(i - imin - 1),
			      j_deltas.at(i - imin)));
	}
    }

    // do columns
    std::vector<std::vector<Delta>> i_deltas;
    std::vector<std::vector<Delta>> i_deltas_f;
    for (size_t j = jmin; j <= jmax; j++) {
	i_deltas.push_back(col_deltas(map, j, imin, imax + 1));
	if (j == jmin) {
	    i_deltas_f.push_back(i_deltas.at(j - jmin));
	} else {
	    i_deltas_f.push_back(
		filter_deltas(i_deltas.at(j - jmin - 1),
			      i_deltas.at(j - jmin)));
	}
    }
    size_t rows = count_deltas(j_deltas_f);
    size_t cols = count_deltas(i_deltas_f);
    // std::cout << rows << "+" << cols << "=" << (rows+cols) << std::endl;
    return rows + cols;
}


uint64_t calculate_price(const Region& r, bool bulk = false)
{
    uint64_t area = r.size();
    uint64_t perimeter = bulk? count_sides(r) : calculate_perimeter(r);
    // std::cout << "Bulk:" << bulk << " Type:" << r.front().type << " Area:" << area << " Perimeter:" << perimeter << " Price:" << (area*perimeter) << std::endl;
    return area * perimeter;
}

uint64_t calculate_price(const std::vector<Region>& rs, bool bulk = false)
{
    return std::accumulate(rs.begin(), rs.end(), 0ull,
			   [=](uint64_t acc, const Region& r){
			       return acc + calculate_price(r, bulk);
			   });
}

Answer part_1(const std::string& input)
{
    return ans(calculate_price(get_regions(input)));
}

Answer part_2(const std::string& input)
{
    return ans(calculate_price(get_regions(input), true));
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
	// 3
	"EEEEE\n"
	"EXXXX\n"
	"EEEEE\n"
	"EXXXX\n"
	"EEEEE\n",
	// 4
	"AAAAAA\n"
	"AAABBA\n"
	"AAABBA\n"
	"ABBAAA\n"
	"ABBAAA\n"
	"AAAAAA\n",

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

    ///////////////////////////////////////
    std::vector<uint64_t> test_prices_2;
    std::transform(test_regions.begin(),
		   test_regions.end(),
		   std::back_inserter(test_prices_2),
		   [](const std::vector<Region>& rs){return calculate_price(rs, true);});
    CHECK(test_prices_2.at(0) == 80);
    CHECK(test_prices_2.at(1) == 436);
    CHECK(test_prices_2.at(2) == 1206);
    CHECK(test_prices_2.at(3) == 236);
    CHECK(test_prices_2.at(4) == 368);

    CHECK(part_2(test_input.at(0)) == 80);
    CHECK(part_2(test_input.at(1)) == 436);
    CHECK(part_2(test_input.at(2)) == 1206);
    CHECK(part_2(test_input.at(3)) == 236);
    CHECK(part_2(test_input.at(4)) == 368);
    
}

} //namespace day12

