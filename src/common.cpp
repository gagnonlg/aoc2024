#include "common.hpp"

/////////////////////////////////////
// String handling

std::vector<std::string> split_at(std::istream& stream, char c, bool allow_empty)
{
    std::vector<std::string> lines;
    std::string buf;
    while(std::getline(stream, buf, c)) {
	if (allow_empty ||
	    std::any_of(buf.begin(), buf.end(), [&](auto x){return x != c;})) {
	    lines.push_back(std::move(buf));
	}
	buf.clear();
    }
    return lines;
}

std::vector<std::string> split_at(const std::string& str, char c, bool allow_empty)
{
      std::stringstream stream(str);
      return split_at(stream, c, allow_empty);
}

std::vector<std::string> split_at(const char *str, char c, bool allow_empty)
{
      std::stringstream stream(str);
      return split_at(stream, c, allow_empty);
}

std::vector<std::string> split_lines(std::istream& stream)
{
    return split_at(stream, '\n', true);
}

std::vector<std::string> split_lines(const std::string& str)
{
      std::stringstream stream(str);
      return split_lines(stream);
}

std::vector<std::string> split_lines(const char *str)
{
      std::stringstream stream(str);
      return split_lines(stream);
}


std::vector<int64_t> split_ints(std::istream& stream, char c)
{
    std::vector<std::string> strs = split_at(stream, c);
    std::vector<int64_t> nums;
    std::transform(strs.begin(), strs.end(), std::back_inserter(nums),
		   [](const auto& s){return std::stoll(s);});
    return nums;
}


std::vector<int64_t> split_ints(const std::string& str, char c)
{
    std::stringstream stream(str);
    return split_ints(stream, c);
}

std::vector<int64_t> split_ints(const char *str, char c)
{
    std::stringstream stream(str);
    return split_ints(stream, c);
}


std::vector<std::vector<std::string>> split_columns(std::istream& stream)
{
    std::vector<std::string> lines = split_lines(stream);

    std::vector<std::vector<std::string>> rows;   
    std::transform(lines.begin(), lines.end(), std::back_inserter(rows),
		   [](const std::string& s){ return split_at(s, ' ', false); });

    if (rows.empty()) {
	return {};
    }

    size_t row_size = rows[0].size();
    std::vector<std::vector<std::string>> columns(row_size);
    std::for_each(rows.begin(), rows.end(), [&](auto& strs){
	CHECK(strs.size() == row_size);
	for (size_t i = 0; i < row_size; i++) {
	    columns[i].push_back(std::move(strs[i]));
	}
    });
    
    return columns;    
}

std::vector<std::vector<std::string>> split_columns(const std::string& str)
{
    std::stringstream stream(str);
    return split_columns(stream);
}

std::vector<std::vector<std::string>> split_columns(const char *str)
{
    std::stringstream stream(str);
    return split_columns(stream);
}

std::vector<std::vector<int64_t>> split_int_columns(std::istream& stream)
{
    std::vector<std::vector<std::string>> columns = split_columns(stream);
    std::vector<std::vector<int64_t>> num_columns;
    std::transform(columns.begin(), columns.end(), std::back_inserter(num_columns),
		   [](const auto& strs){
		       std::vector<int64_t> nums;
		       std::transform(strs.begin(), strs.end(),
				      std::back_inserter(nums),
				      [](const auto& s){
					  return stoll(s); });
		       return nums;
		   });
    return num_columns;
}

std::vector<std::vector<int64_t>> split_int_columns(const std::string& str)
{
    std::stringstream stream(str);
    return split_int_columns(stream);
}

std::vector<std::vector<int64_t>> split_int_columns(const char *str)
{
    std::stringstream stream(str);
    return split_int_columns(stream);
}

std::vector<std::vector<int64_t>> split_int_lines(std::istream& stream)
{
    std::vector<std::string> lines = split_lines(stream);
    std::vector<std::vector<int64_t>> int_lines;
    std::transform(std::begin(lines), std::end(lines),
		   std::back_inserter(int_lines), [](const std::string& s) {
		       return split_ints(s);
		   });
    return int_lines;
}

std::vector<std::vector<int64_t>> split_int_lines(const std::string& str)
{
    std::stringstream stream(str);
    return split_int_lines(stream);
}

std::vector<std::vector<int64_t>> split_int_lines(const char *str)
{
    std::stringstream stream(str);
    return split_int_lines(stream);
}

std::vector<std::vector<std::string>> split_line_blocks(std::istream& stream)
{
    std::vector<std::string> lines = split_lines(stream);
    std::vector<std::vector<std::string>> blocks;
    std::vector<std::string> block;

    for (const std::string& line : lines) {
	if (line.empty()) {
	    blocks.push_back(std::move(block));
	    block.clear();
	} else {
	    block.push_back(std::move(line));
	}
    }
    if (!block.empty())
	blocks.push_back(std::move(block));

    return blocks;
}

std::vector<std::vector<std::string>> split_line_blocks(const std::string& str)
{
    std::stringstream stream(str);
    return split_line_blocks(stream);
}

