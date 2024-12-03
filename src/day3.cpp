#include <numeric>

#include "common.hpp"

namespace day3 {

std::vector<size_t> find_all_indices(const std::string& input,
				     const std::string& pattern)
{
    std::vector<size_t> idx;
    size_t offset = 0;

    while (offset < input.size()) {
	size_t pos = input.find(pattern, offset);
	if (pos != std::string::npos) {
	    idx.push_back(pos);
	    offset = pos + pattern.size();
	} else {
	    break;
	}
    }

    return idx;
}

std::vector<std::pair<size_t, size_t>> find_enabled_ranges(const std::string& input)
{
    std::vector<size_t> i_do = find_all_indices(input, "do()");
    std::vector<size_t> i_dont = find_all_indices(input, "don't()");

    CHECK(!i_do.empty());
    CHECK(!i_dont.empty());
    CHECK(i_do[0] != 0);
    CHECK(i_dont[0] != 0);

    std::vector<std::pair<size_t, size_t>> ranges;
    size_t low = 0;
    while (true) {
	auto it1 = std::upper_bound(i_dont.begin(), i_dont.end(), low);
	if (it1 != i_dont.end()) {
	    ranges.emplace_back(low, *it1);
	} else {
	    // Stop cond 1: Valid up to the end
	    ranges.emplace_back(low, input.size());
	    break;
	}

	auto it2 = std::upper_bound(i_do.begin(), i_do.end(), *it1);
	if (it2 != i_do.end()) {
	    low = *it2;
	} else {
	    // Stop cond 2: Never re-enabled
	    break;
	}
    }
    return ranges;
}


std::vector<size_t> find_mul_indices(const std::string& input)
{
    return find_all_indices(input, "mul(");
}

std::string leading_digits(const std::string& input, size_t offset)
{
    std::string digits;
    for (size_t i = offset; i < input.size(); i++) {
	char c = input[i];
	if (std::isdigit(static_cast<unsigned char>(c)))
	    digits.push_back(c);
	else
	    break;
    }
    return digits;
}

std::optional<int64_t> mul_at(const std::string& input, size_t offset)
{
    CHECK(input.substr(offset, 4) == "mul(");
    offset += 4;

    // First seq of digits
    std::string str_lhs = leading_digits(input, offset);
    offset += str_lhs.size();
    if (str_lhs.empty())
	return {};

    // single ,
    if (offset >= input.size() || input[offset] != ',')
	return {};
    offset += 1;
    
    // Second seq of digits
    std::string str_rhs = leading_digits(input, offset);
    offset += str_rhs.size();
    if (str_rhs.empty())
	return {};

    // single ,
    if (offset >= input.size() || input[offset] != ')')
	return {};

    // Valid mul!
    return stoll(str_lhs) * stoll(str_rhs);
}

std::vector<int64_t> all_muls(const std::string& input)
{
    std::vector<int64_t> res;
    for (size_t idx : find_mul_indices(input)) {
	std::optional<int64_t> o = mul_at(input, idx);
	if (o) {
	    res.push_back(*o);
	}
    }
    return res;
}

std::vector<size_t> filter_indices(
    const std::vector<size_t>& all_indices,
    const std::vector<std::pair<size_t, size_t>>& ranges)
{
    std::vector<size_t> indices;
    size_t i_range = 0;
    size_t i_idx = 0;

    while (i_idx < all_indices.size() && i_range < ranges.size()) {

	size_t xlow = ranges[i_range].first;
	size_t xup = ranges[i_range].second;
	size_t x = all_indices[i_idx];

	// is x below range? just keep looking!
	if (x < xlow) {
	    i_idx++;
	    continue;
	}
	// is x above range? get the next range!
	if (x >= xup) {
	    i_range++;
	    continue;
	}

	// else, in range!
	indices.push_back(x);
	i_idx++;
    }
    return indices;
}

std::vector<int64_t> all_muls_with_conds(const std::string& input)
{
    std::vector<std::pair<size_t, size_t>> ranges = find_enabled_ranges(input);
    std::vector<size_t> indices = filter_indices(find_mul_indices(input), ranges);

    
    std::vector<int64_t> res;
    for (size_t idx : indices) {
	std::optional<int64_t> o = mul_at(input, idx);
	if (o) {
	    res.push_back(*o);
	}
    }
    return res;
}

Answer part_1(const std::string& input)
{
    std::vector<int64_t> muls = all_muls(input);
    return std::accumulate(muls.begin(), muls.end(), 0);
}

Answer part_2(const std::string& input)
{
    std::vector<int64_t> muls = all_muls_with_conds(input);
    return std::accumulate(muls.begin(), muls.end(), 0);
}


void tests()
{
    CHECK(find_mul_indices("mul(44,46)mul(44,46)") ==
	  std::vector<size_t>({0,10}));
    CHECK(find_mul_indices("what(),where()when()>mul(") ==
	  std::vector<size_t>({21}));

    CHECK(mul_at("mul(44,46)", 0) == (44*46));
    CHECK(mul_at("mul(123,4)", 0) == (123*4));
    CHECK(mul_at("what(),where()when()>mul(123,4)", 21) == (123*4));

    std::string bads = "mul(4*mul(6,9!?(12,34)mul ( 2 , 4 )";
    for (size_t i : find_mul_indices(bads)) {
	CHECK(!mul_at(bads, i));
    }

    std::string ex =
	"xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";
    CHECK(all_muls(ex) == std::vector<int64_t>({2*4,5*5,11*8,8*5}));
    CHECK(part_1(ex) == 161);

    ////////////////////////////////////
    
    ex = "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))";
    std::vector<std::pair<size_t, size_t>> ranges = find_enabled_ranges(ex);
    CHECK(ranges.size() == 2);
    CHECK(ranges[0].first == 0);
    CHECK(ranges[0].second == 20);
    CHECK(ranges[1].first == 59);
    CHECK(ranges[1].second == 73);

    CHECK(filter_indices(find_mul_indices(ex), find_enabled_ranges(ex))
	  == std::vector<size_t>({1,64}));

    CHECK(all_muls_with_conds(ex) == std::vector<int64_t>({2*4,8*5}));

    CHECK(part_2(ex) == 48);
    
}

} //namespace day3

