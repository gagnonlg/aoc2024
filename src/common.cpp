#include "common.hpp"

/////////////////////////////////////
// String handling

std::vector<std::string> split_at(std::istream& stream, char c)
{
    std::vector<std::string> lines;
    std::string buf;
    while(std::getline(stream, buf, c)) {
	lines.push_back(std::move(buf));
	buf.clear();
    }
    return lines;
}

std::vector<std::string> split_at(const std::string& str, char c)
{
      std::stringstream stream(str);
      return split_at(stream, c);
}

std::vector<std::string> split_at(const char *str, char c)
{
      std::stringstream stream(str);
      return split_at(stream, c);
}

std::vector<std::string> split_lines(std::istream& stream)
{
    return split_at(stream, '\n');
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


std::vector<int64_t> split_ints(std::istream& stream)
{
    std::vector<std::string> strs = split_at(stream, ' ');
    std::vector<int64_t> nums;
    std::transform(strs.begin(), strs.end(), std::back_inserter(nums),
		   [](const auto& s){return std::stoll(s);});
    return nums;
}


std::vector<int64_t> split_ints(const std::string& str)
{
    std::stringstream stream(str);
    return split_ints(stream);
}

std::vector<int64_t> split_ints(const char *str)
{
    std::stringstream stream(str);
    return split_ints(stream);
}
