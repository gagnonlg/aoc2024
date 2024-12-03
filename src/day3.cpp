#include <numeric>

#include "common.hpp"

namespace day3 {

std::vector<size_t> find_mul_indices(const std::string& input)
{
    std::vector<size_t> idx;
    size_t offset = 0;
    while (true) {
	size_t pos = input.find("mul(", offset);
	if (pos != std::string::npos) {
	    idx.push_back(pos);
	    offset = pos + 4;
	} else {
	    break;
	}
    }
    return idx;
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

Answer part_1(const std::string& input)
{
    std::vector<int64_t> muls = all_muls(input);
    return std::accumulate(muls.begin(), muls.end(), 0);
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
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
}

} //namespace day3

