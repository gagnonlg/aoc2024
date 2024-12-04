#include <unordered_map>

#include "common.hpp"

namespace day4 {

class Grid {
public:
    Grid(size_t n_rows, size_t row_size)
	: m_n_rows{n_rows},
	  m_row_size{row_size},
	  m_chars(n_rows * row_size, '\0')
	{}

    void insert(size_t i, size_t j, char c) {
	m_chars[checked_idx(i, j)] = c;
    }

    void insert(const std::vector<std::string>& lines) {
	for (size_t i = 0; i < lines.size(); i++) {
	    for (size_t j = 0; j < lines[i].size(); j++) {
		insert(i, j, lines[i][j]);
	    }
	}
    }

    char at(size_t idx) const {
 	char c = m_chars[idx];
	CHECK(c != '\0');
	return c;
    }

    char at(size_t i, size_t j) const {
 	return at(checked_idx(i, j));
    }

    char at(const std::pair<size_t, size_t> coords) const {
	return at(coords.first, coords.second);
    }


    
    size_t rows() const { return m_n_rows; }
    size_t cols() const { return m_row_size; }

    size_t checked_idx(size_t i, size_t j) const {
	size_t idx = i * m_row_size + j;
	CHECK(j < m_row_size);
	CHECK(idx < m_chars.size());
	return idx;
    }

private:
    size_t m_n_rows;
    size_t m_row_size;
    std::vector<char> m_chars;
};

std::pair<Grid, std::vector<std::pair<size_t, size_t>>>
make_grid_and_roots(const std::vector<std::string>& lines)
{
    CHECK(!lines.empty());
    Grid grid(lines.size(), lines[0].size());
    std::vector<std::pair<size_t, size_t>> roots;
    for (size_t i = 0; i < lines.size(); i++) {
	for (size_t j = 0; j < lines[i].size(); j++) {
	    grid.insert(i, j, lines[i][j]);
	    if (lines[i][j] == 'X') {
		roots.emplace_back(i, j);
	    }
	}
    }
    return std::make_pair(grid, roots);
}

std::pair<char, char> get_fwd_bwd_chars(char c)
{
    switch (c) {
    case 'X': return std::make_pair('M',  '\0');
    case 'M': return std::make_pair('A',  'X');
    case 'A': return std::make_pair('S',  'M');
    case 'S': return std::make_pair('\0', 'A');
    default: throw std::runtime_error("INVALID CHAR");
    }
}

struct Node {
    std::optional<size_t> fwd{};
    std::optional<size_t> bwd{};
};

void check_conn(size_t cur,
		size_t next,
		const std::pair<char, char>& fwd_bwd,
		const Grid& grid,
		std::unordered_map<size_t, Node>& node_map)
{
    if (grid.at(next) == fwd_bwd.first)
	node_map[cur].fwd = next;
    else if (grid.at(next) == fwd_bwd.second)
	node_map[cur].bwd = next;
}


std::pair<std::unordered_map<size_t, Node>,
	  std::vector<size_t>>
node_map_and_roots(const Grid& grid)
{
    std::unordered_map<size_t, Node> nodes;
    std::vector<size_t> roots;

    for (size_t i = 0; i < grid.rows(); i++) {
	for (size_t j = 0; j < grid.cols(); j++) {
	    size_t cur = grid.checked_idx(i, j);

	    char c = grid.at(cur);

	    if (c == 'X' || c == 'S') {
		roots.push_back(cur);
	    }

	    std::pair<char, char> fwd_bwd = get_fwd_bwd_chars(c);

 	    // 1: Right
	    if (j < (grid.cols() - 1)) {
		check_conn(cur, grid.checked_idx(i, j + 1), fwd_bwd, grid, nodes);
	    }
 	    // 2: Down, Right
	    if ((i < (grid.rows() - 1)) && (j < (grid.cols() - 1))) {
		check_conn(cur, grid.checked_idx(i + 1,j + 1), fwd_bwd, grid, nodes);
	    }
 	    // 3: Down
	    if (i < (grid.rows() - 1)) { 
		check_conn(cur, grid.checked_idx(i + 1, j), fwd_bwd, grid, nodes);
	    }
	    // 4: Down, right
	    if ((i < (grid.rows() - 1)) && (j > 0)) {
		check_conn(cur, grid.checked_idx(i + 1,j - 1), fwd_bwd, grid, nodes);	       }
	}
    }
    return std::make_pair(nodes, roots);
}

// bool xmas_at(size_t root,
// 	     const Grid& grid,
// 	     const std::unordered_map<size_t, Node>& node_map)
// {

//     if (!node_map.contains(root))
// 	return false;

//     char c = grid.at(root);
//     bool fwd = c == 'X';
//     const char * pat = fwd ? "XMAS" : "SAMX";

//     std::optional<size_t> opt_i = root;
//     size_t pat_i = 0;
//     bool valid = true;
//     while (opt_i && valid && pat_i < 4) {
// 	std::cout<<grid.at(*opt_i)<<" "<<pat[pat_i]<<std::endl;
// 	if (grid.at(*opt_i) != pat[pat_i]) {
// 	    valid = false;
// 	} else {
// 	    opt_i = fwd ? node_map.at(*opt_i).fwd : node_map.at(*opt_i).bwd;
// 	    pat_i += 1;
// 	}
//     }
//     return valid && (pat_i == 4);
// }

size_t xmas_at(size_t i, size_t j, const Grid& grid)
{
    size_t n_xmas = 0;
    
    const char * pat_fwd = "XMAS";
    const char * pat_bwd = "SAMX";
    char const *pat;
    char c = grid.at(i, j);
    if (c == 'X')
	pat = pat_fwd;
    else if (c == 'S')
	pat = pat_bwd;
    else
	return false;

    // Straight line to left
    size_t n = 0;
    for (size_t offset = 0;
	 (j + offset) < grid.cols() && offset < 4;
	 offset++) {
	if (grid.at(i, j + offset) != pat[offset])
	    break;
	else
	    n++;
    }
    if (n == 4) {
	n_xmas += 1;
    }

    // Straight down
    n = 0;
    for (size_t offset = 0;
	 (i + offset) < grid.rows()  && offset < 4;
	 offset++) {
	if (grid.at(i + offset, j) != pat[offset])
	    break;
	else
	    n++;
    }
    if (n == 4) {
	n_xmas += 1;
    }

    // Diagonal right
    n = 0;
    for (size_t offset = 0;
	 (i + offset) < grid.rows() && (j + offset < grid.cols()) && offset < 4;
	 offset++) {
	if (grid.at(i + offset, j + offset) != pat[offset])
	    break;
	else
	    n++;
    }
    if (n == 4) {
	n_xmas += 1;
    }

    // Diagonal left
    n = 0;
    for (size_t offset = 0;
	 (i + offset) < grid.rows() && (j >= offset) && offset < 4;
	 offset++) {
	if (grid.at(i + offset, j - offset) != pat[offset])
	    break;
	else
	    n++;
    }
    if (n == 4) {
	n_xmas += 1;
    }

    return n_xmas;
}

size_t x_mas_at(size_t i, size_t j, const Grid& grid)
{
    if (i == 0 || i == (grid.rows() - 1))
	return false;
    if (j == 0 || j == (grid.cols() - 1))
	return false;
    if (grid.at(i, j) != 'A')
	return false;

    char x = grid.at(i - 1, j - 1);
    char y = grid.at(i + 1, j + 1);
    if (!(((x == 'M') && (y == 'S')) || ((x == 'S') && (y == 'M'))))
	return false;

    x = grid.at(i - 1, j + 1);
    y = grid.at(i + 1, j - 1);
    if (!(((x == 'M') && (y == 'S')) || ((x == 'S') && (y == 'M'))))
	return false;
	  
    return true;
}


Answer part_1(const std::string& input)
{
    std::vector<std::string> lines = split_lines(input);
    Grid grid(lines.size(), lines[0].size());
    grid.insert(lines);

    size_t n = 0;
    for (size_t i = 0; i < grid.rows(); i++) {	
	for (size_t j = 0; j < grid.cols(); j++) {
	    n += xmas_at(i, j, grid);
	}
    }
    return ans(n);
}

Answer part_2(const std::string& input)
{
    std::vector<std::string> lines = split_lines(input);
    Grid grid(lines.size(), lines[0].size());
    grid.insert(lines);

    size_t n = 0;
    for (size_t i = 0; i < grid.rows(); i++) {	
	for (size_t j = 0; j < grid.cols(); j++) {
	    if (x_mas_at(i, j, grid))
		n++;
	}
    }
    return ans(n);
}

void tests()
{
    const char * test_input_1 =
	"MMMSXXMASM\n"
	"MSAMXMSMSA\n"
	"AMXSXMAAMM\n"
	"MSAMASMSMX\n"
	"XMASAMXAMM\n"
	"XXAMMXXAMA\n"
	"SMSMSASXSS\n"
	"SAXAMASAAA\n"
	"MAMMMXMMMM\n"
	"MXMXAXMASX\n";

    std::vector<std::string> lines = split_lines(test_input_1);
    Grid grid(lines.size(), lines[0].size());
    grid.insert(lines);
    CHECK(grid.at(0, 0) == 'M');
    CHECK(grid.at(9, 9) == 'X');
    CHECK(xmas_at(0, 4, grid) == 1);
    CHECK(xmas_at(0, 5, grid) == 1);
    CHECK(xmas_at(1, 1, grid) == 1);
    CHECK(xmas_at(1, 6, grid) == 1);
    CHECK(xmas_at(2, 3, grid) == 2);
    CHECK(xmas_at(3, 9, grid) == 2);
    CHECK(xmas_at(4, 0, grid) == 1);
    CHECK(xmas_at(4, 3, grid) == 1);
    CHECK(xmas_at(6, 0, grid) == 1);
    CHECK(xmas_at(6, 2, grid) == 1);
    CHECK(xmas_at(6, 4, grid) == 1);
    CHECK(xmas_at(6, 6, grid) == 2);
    CHECK(xmas_at(6, 8, grid) == 1);
    CHECK(xmas_at(6, 9, grid) == 1);
    CHECK(xmas_at(9, 5, grid) == 1);

    CHECK(part_1(test_input_1) == 18);

    /////////////////////////////

    CHECK(x_mas_at(1, 2, grid));
    CHECK(part_2(test_input_1) == 9);
}

} //namespace day4

