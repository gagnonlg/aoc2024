#include <set>
#include <numeric>
#include <random>
#include "common.hpp"

namespace day7 {

struct InputData {
    uint64_t result;
    std::vector<uint64_t> operands;
};

std::vector<InputData> read_input_data(const std::string& input)
{
    std::vector<InputData> retv;
    std::vector<std::string> lines = split_lines(input);    
    for (const std::string& line : lines) {
	InputData data;
	std::vector<std::string> fields = split_at(line, ':');
	CHECK(fields.size() == 2);
	data.result = stoull(fields[0]);
	std::vector<std::string> numstrs = split_at(fields[1], ' ');
	std::transform(numstrs.begin(), numstrs.end(),
		       std::back_inserter(data.operands),
		       [](auto s){return stoull(s);});
	retv.push_back(std::move(data));
    }

    return retv;
}

enum Operator { Add, Mul };
enum Result {Low, Equal, High};

Result eval(const InputData& data, const std::vector<Operator>& operators)
{
    CHECK(data.operands.size() >= 2);
    CHECK(operators.size() == data.operands.size() - 1);

    uint64_t acc = data.operands[0];

    for (size_t i = 1; i < data.operands.size(); i++) {
	switch (operators.at(i - 1)) {
	case Add:
	    acc = acc + data.operands[i];
	    break;
	case Mul:
	    acc = acc * data.operands[i];
	    break;
	}
    }

    if (acc > data.result)
	return High;
    if (acc < data.result)
	return Low;
    return Equal;
}

bool produced_by(const InputData& data, const std::vector<Operator>& operators)
{
    return eval(data, operators) == Equal;
}

bool is_valid_data(const InputData& data, std::vector<Operator>& operators, size_t i, size_t& count, std::set<std::vector<Operator>>& seen)
{
    if (i == operators.size()) {
	CHECK(seen.insert(operators).second);
	count++;
	return produced_by(data, operators);
    }

    operators[i] = Mul;
    bool p = false;
    if (is_valid_data(data, operators, i + 1, count, seen))
	// return true;
	p = true;

    operators[i] = Add;
    if (is_valid_data(data, operators, i + 1, count, seen))
	// return true;
	p = true;

    // return false;
    return p;
}

void print_data(const InputData& data)
{
    std::cout << "data.result: " << data.result << std::endl;
    std::cout << "data.operands: ";
    for (auto num : data.operands)
	std::cout << num << " ";
    std::cout << std::endl;
}

bool is_valid_data(const InputData& data)
{
    std::vector<Operator> operators(data.operands.size() - 1, Operator::Mul);
    
    size_t count = 0;
    std::set<std::vector<Operator>> seen;
    bool v = is_valid_data(data, operators, 0, count, seen);
    CHECK(count == (1u << operators.size()));
    return v;
}

void checker_part1(int trials = 100000)
{
    std::random_device dev;
    std::mt19937 rng(dev());

    std::uniform_int_distribution<std::mt19937::result_type> dist_op(0, 1);
    std::uniform_int_distribution<std::mt19937::result_type> dist_n(2, 10);
    std::uniform_int_distribution<std::mt19937::result_type> dist_num(0, 100);

    while (trials-->0) {

	InputData data;
	
	uint64_t n = dist_n(rng);

	for (size_t i = 0; i < n; i++)
	    data.operands.push_back(dist_num(rng));

	std::uniform_int_distribution<std::mt19937::result_type> dist_1(0, n);
	size_t i_1 = dist_1(rng);
	if (i_1 < n)
	    data.operands[i_1] = 1;

	
	CHECK(!data.operands.empty());
	data.result = data.operands[0];

	std::vector<Operator> ops;
	for (size_t i = 0; i < (n - 1); i++) {
	    switch (dist_op(rng)) {
	    case 0:
		data.result = data.result + data.operands[i + 1];
		ops.push_back(Add);
		break;
	    case 1:
		data.result = data.result * data.operands[i + 1];
		ops.push_back(Mul);
		break;
	    default:
		throw std::runtime_error("impossible");
	    }
	}

	if (!is_valid_data(data)) {
	    std::cout << "*** ERROR DETECTED BY CHECKER!" << std::endl;
	    std::cout << "data.result: " << data.result << std::endl;
	    std::cout << "data.operands: ";
	    for (auto num : data.operands)
		std::cout << num << " ";
	    std::cout << std::endl;
	    std::cout << "Operators: ";
	    for (auto op : ops)
		std::cout << (op == Add ? "+" : "*") << " ";
	    std::cout << std::endl;	    
	}

	CHECK(is_valid_data(data));
	CHECK(produced_by(data, ops));
    }
}


Answer part_1(const std::string& input)
{
    std::vector<InputData> data = read_input_data(input);
    uint64_t acc = std::accumulate(data.begin(), data.end(), 0ull,
			   [](uint64_t a, const InputData& d){
			       if (is_valid_data(d))
				   return a + d.result;
			       else {
				   return a;
			       }
			   });
    return ans(acc);
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
}

void tests()
{
    const char * test_input_1 =
	"190: 10 19\n"
	"3267: 81 40 27\n"
	"83: 17 5\n"
	"156: 15 6\n"
	"7290: 6 8 6 15\n"
	"161011: 16 10 13\n"
	"192: 17 8 14\n"
	"21037: 9 7 18 13\n"
	"292: 11 6 16 20\n";

    std::vector<InputData> data = read_input_data(test_input_1);
    CHECK(data.size() == 9);
    CHECK(data[0].result == 190);
    CHECK(data[0].operands == std::vector<uint64_t>({10,19}));
    CHECK(data[1].result == 3267);
    CHECK(data[1].operands == std::vector<uint64_t>({81,40,27}));
    CHECK(data[2].result == 83);
    CHECK(data[2].operands == std::vector<uint64_t>({17,5}));
    CHECK(data[3].result == 156);
    CHECK(data[3].operands == std::vector<uint64_t>({15,6}));
    CHECK(data[4].result == 7290);
    CHECK(data[4].operands == std::vector<uint64_t>({6,8,6,15}));
    CHECK(data[5].result == 161011);
    CHECK(data[5].operands == std::vector<uint64_t>({16,10,13}));
    CHECK(data[6].result == 192);
    CHECK(data[6].operands == std::vector<uint64_t>({17,8,14}));
    CHECK(data[7].result == 21037);
    CHECK(data[7].operands == std::vector<uint64_t>({9,7,18,13}));
    CHECK(data[8].result == 292);
    CHECK(data[8].operands == std::vector<uint64_t>({11,6,16,20}));

    CHECK(!produced_by(data[0], {Add}));
    CHECK(produced_by(data[0], {Mul}));
    CHECK(produced_by(data[1], {Add, Mul}));
    CHECK(produced_by(data[1], {Mul, Add}));
    CHECK(produced_by(data[8], {Add, Mul, Add}));

    CHECK(is_valid_data(data[0]));
    CHECK(is_valid_data(data[1]));
    CHECK(!is_valid_data(data[2]));
    CHECK(!is_valid_data(data[3]));
    CHECK(!is_valid_data(data[4]));
    CHECK(!is_valid_data(data[5]));
    CHECK(!is_valid_data(data[6]));
    CHECK(!is_valid_data(data[7]));
    CHECK(is_valid_data(data[8]));

    // checker_part1();
    
    // Found by checker
    InputData my_data = {
	58,
	std::vector<uint64_t>({75,19,37,56,0,81,32,49,58})
    };
    std::vector<Operator> my_ops = {Mul,Mul,Mul,Mul,Mul,Mul,Mul,Add};
    CHECK(produced_by(my_data, my_ops));
    CHECK(is_valid_data(my_data));

    CHECK(part_1(test_input_1) == 3749);

   
}

} //namespace day7

