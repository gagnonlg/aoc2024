#include <cmath>
#include <numeric>
#include <regex>
#include "common.hpp"

namespace day13 {

double sdiv(double x, double y)
{
    return std::exp(std::log(x) - std::log(y));
}

double smul(double x, double y)
{
    return std::exp(std::log(x) + std::log(y));
}

struct System {
    double nA;
    double nB;
    double xA;
    double yA;
    double xB;
    double yB;
    double x;
    double y;

    void solve() {
	double t0 = y / yB;
	double t1 = yA / (xA * yB);
	nB = (t0 - x * t1)/(1 - xB * t1);
	nA = (x - nB * xB) / xA;
    }
    // void solve() {
    // 	double t0 = sdiv(y, yB);
    // 	double t1 = sdiv(yA, smul(xA , yB));
    // 	nB = sdiv(t0 - smul(x, t1), 1 - smul(xB, t1));
    // 	nA = sdiv(x - smul(nB , xB), xA);
    // }
    
    System(double _xA, double _yA,
	   double _xB, double _yB,
	   double _x, double _y)
	: xA(_xA), yA(_yA),
	  xB(_xB), yB(_yB),
	  x(_x), y(_y) {
	solve();
    }

    bool solved_by(double N, double M) const {
	double x_ = N * xA + M * xB;
	double y_ = N * yA + M * yB;
	double e = 1e-6;
	return std::abs(x_ - x) < e && std::abs(y_ - y) < e;
    }

    bool solved() const {
	return solved_by(nA, nB);
    }

    std::optional<std::pair<int64_t, int64_t>>
    integer_solution(double max = 100) const {
	double N = std::round(nA);
	double M = std::round(nB);
	if (solved_by(N, M) && N >= 0 && M >= 0 && N <= max && M <= max) {
	    return std::make_pair(
		static_cast<int64_t>(N),
		static_cast<int64_t>(M));
	} else {
	    return {};
	}
    }

    std::optional<int64_t> cost(double max = 100) const {
	auto coeffs = integer_solution(max);
	if (!coeffs)
	    return {};
	return coeffs->first * 3 + coeffs->second * 1;
    }
};

std::pair<double, double> read_pair(const std::string& str,
				    const std::regex& re)
{
    std::smatch m;
    CHECK(std::regex_match(str, m, re));
    CHECK(m.size() == 3);
    return std::make_pair(std::stod(m[1].str()),
			  std::stod(m[2].str()));
}

System read_system(const std::vector<std::string>& strs)
{
    CHECK(strs.size() == 3);

    std::regex re0("Button A: X\\+([0-9]+), Y\\+([0-9]+)");
    std::regex re1("Button B: X\\+([0-9]+), Y\\+([0-9]+)");
    std::regex re2("Prize: X=([0-9]+), Y=([0-9]+)");
    auto [xA, yA] = read_pair(strs[0], re0);
    auto [xB, yB] = read_pair(strs[1], re1);
    auto [x, y] = read_pair(strs[2], re2);
    return System(xA, yA, xB, yB, x, y);
}

std::vector<System> read_input_data(const std::string& input)
{
    std::vector<std::vector<std::string>> blocks = split_line_blocks(input);
    std::vector<System> syss;
    std::transform(blocks.begin(), blocks.end(), std::back_inserter(syss),
		   read_system);
    return syss;
}

int64_t count_tokens(const std::vector<System>& systems)
{
    return std::accumulate(systems.begin(), systems.end(), 0ll,
			   [](int64_t acc, const System& sys){
			       if (sys.cost().has_value())
				   return acc + sys.cost().value();
			       else
				   return acc;
			   });
}

double fixd(double d)
{
    std::string lhs = "10000000000000";
    std::string rhs = std::to_string(static_cast<int64_t>(std::round(d)));
    // std::cout << (lhs+rhs) << std::endl; // 
    return std::stod(lhs + rhs);
}

void fix(System& sys)
{
    
    // std::cout << sys.x << std::endl;
    // std::cout << sys.y << std::endl;
    sys.x = fixd(sys.x);
    sys.y = fixd(sys.y);
    sys.solve();
    // std::cout << sys.xA << std::endl;
    // std::cout << sys.yB << std::endl;
    // std::cout << sys.xB << std::endl;
    // std::cout << sys.yB << std::endl;
    // std::cout << sys.nA << std::endl;
    // std::cout << sys.nB << std::endl;
    // std::cout << sys.x << std::endl;
    // std::cout << sys.y << std::endl;
    // std::cout << (sys.xA * sys.nA + sys.xB * sys.nB) << std::endl;
    // std::cout << (sys.yA * sys.nA + sys.yB * sys.nB) << std::endl;
    // std::cout << ((sys.xA * sys.nA + sys.xB * sys.nB) - sys.x) << std::endl;
    // std::cout << ((sys.yA * sys.nA + sys.yB * sys.nB) - sys.y) << std::endl;
    
    CHECK(sys.solved());
}

void fix_systems(std::vector<System>& systems)
{
    std::for_each(systems.begin(), systems.end(), fix);
}

Answer part_1(const std::string& input)
{
    std::vector<System> systems = read_input_data(input);
    return count_tokens(systems);
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
}

void tests()
{
    System sys(94, 34, 22, 67, 8400, 5400);
    CHECK(sys.solved());
    CHECK(sys.integer_solution());
    CHECK(sys.integer_solution()->first == 80);
    CHECK(sys.integer_solution()->second == 40);
    CHECK(sys.cost() && sys.cost() == 280);

    sys = System(26, 66, 67, 21, 12748, 12176);
    CHECK(sys.solved());
    CHECK(!sys.integer_solution());

    sys = System(17, 86, 84, 37, 7870, 6450);
    CHECK(sys.solved());
    CHECK(sys.integer_solution());
    CHECK(sys.integer_solution()->first == 38);
    CHECK(sys.integer_solution()->second == 86);
    CHECK(sys.cost() && sys.cost() == 200);

    sys = System(69, 23, 27, 71, 18641, 10279);
    CHECK(sys.solved());
    CHECK(!sys.integer_solution());

    const char * test_input_1 =
	"Button A: X+94, Y+34\n"
	"Button B: X+22, Y+67\n"
	"Prize: X=8400, Y=5400\n"
	"\n"
	"Button A: X+26, Y+66\n"
	"Button B: X+67, Y+21\n"
	"Prize: X=12748, Y=12176\n"
	"\n"
	"Button A: X+17, Y+86\n"
	"Button B: X+84, Y+37\n"
	"Prize: X=7870, Y=6450\n"
	"\n"
	"Button A: X+69, Y+23\n"
	"Button B: X+27, Y+71\n"
	"Prize: X=18641, Y=10279\n";

    std::vector<System> systems = read_input_data(test_input_1);

    CHECK(systems.at(0).xA == 94);
    CHECK(systems.at(0).yA == 34);
    CHECK(systems.at(0).xB == 22);
    CHECK(systems.at(0).yB == 67);
    CHECK(systems.at(0).x == 8400);
    CHECK(systems.at(0).y == 5400);
    
    CHECK(systems.at(0).solved());
    CHECK(systems.at(0).integer_solution());
    CHECK(systems.at(0).integer_solution()->first == 80);
    CHECK(systems.at(0).integer_solution()->second == 40);
    CHECK(systems.at(0).cost() && systems.at(0).cost() == 280);

    CHECK(systems.at(1).solved());
    CHECK(!systems.at(1).integer_solution());

    CHECK(systems.at(2).solved());
    CHECK(systems.at(2).integer_solution());
    CHECK(systems.at(2).integer_solution()->first == 38);
    CHECK(systems.at(2).integer_solution()->second == 86);
    CHECK(systems.at(2).cost() && systems.at(2).cost() == 200);

    CHECK(systems.at(3).solved());
    CHECK(!systems.at(3).integer_solution());

    CHECK(count_tokens(systems) == 480);
    CHECK(part_1(test_input_1) == 480);

    ///////////////////////////////////

    // fix_systems(systems);
    // double lim = std::numeric_limits<double>::max();
    // CHECK(!systems.at(0).integer_solution(lim));

}

} //namespace day13

