#include <unordered_map>
#include <unordered_set>

#include "common.hpp"

namespace day5 {

std::pair<std::unordered_map<int64_t, std::unordered_set<int64_t>>,
	  std::vector<std::vector<int64_t>>>
    parse_data(const std::string& str)
{
    std::vector<std::string> lines = split_lines(str);
    size_t i = 0;
    
    std::unordered_map<int64_t, std::unordered_set<int64_t>> map;
    for (; i < lines.size() && !lines[i].empty(); i++) {
	std::vector<int64_t> fields = split_ints(lines[i], '|');
	CHECK(map[fields[0]].insert(fields[1]).second);
    }
    CHECK(!map.empty());
    
    std::vector<std::vector<int64_t>> list;
    for (++i; i < lines.size(); i++) {
	list.push_back(split_ints(lines[i], ','));
    }
    CHECK(!list.empty());

    return std::make_pair(map, list);
}

class MapComp {
public:
    explicit
    MapComp(const std::unordered_map<int64_t, std::unordered_set<int64_t>>& map)
    : m_map{map}
    {}

    bool operator()(int64_t lhs, int64_t rhs) {
	return (lhs != rhs) && m_map.contains(lhs) && m_map.at(lhs).contains(rhs);
    }
    
private:
    const std::unordered_map<int64_t, std::unordered_set<int64_t>>& m_map;  
};

bool is_ordered(const std::vector<int64_t>& list, const MapComp& comp)
{
    return std::is_sorted(list.begin(), list.end(), comp);
}

std::optional<int64_t> middle_if_ordered(
    const std::vector<int64_t>& list, const MapComp& comp)
{
    if (is_ordered(list, comp)) {
	CHECK(list.size() % 2 == 1);
	int64_t half_size = static_cast<int64_t>(list.size() / 2); 
	auto mid = list.begin() + half_size;
	return *mid;
    } else {
	return {};
    }
}

Answer part_1(const std::string& input)
{
    auto [map, lists] = parse_data(input);
    MapComp comp(map);
    Answer acc = 0;
    for (const std::vector<int64_t>& xs : lists) {
	std::optional<int64_t> x = middle_if_ordered(xs, comp);
	if (x) {
	    acc += *x;
	}
    }
    return acc;
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
}

void tests()
{
    const char * test_input_1 =
	"47|53\n"
	"97|13\n"
	"97|61\n"
	"97|47\n"
	"75|29\n"
	"61|13\n"
	"75|53\n"
	"29|13\n"
	"97|29\n"
	"53|29\n"
	"61|53\n"
	"97|53\n"
	"61|29\n"
	"47|13\n"
	"75|47\n"
	"97|75\n"
	"47|61\n"
	"75|61\n"
	"47|29\n"
	"75|13\n"
	"53|13\n"
	"\n"
	"75,47,61,53,29\n"
	"97,61,53,29,13\n"
	"75,29,13\n"
	"75,97,47,61,53\n"
	"61,13,29\n"
	"97,13,75,29,47\n";

    auto [map, list] = parse_data(test_input_1);
    CHECK(map.size() == 6);
    CHECK(map[47].contains(53));
    CHECK(map[47].contains(61));
    CHECK(list.size() == 6);
    CHECK(list[0].size() == 5);

    MapComp comp(map);
    CHECK(comp(97, 13));
    CHECK(comp(97, 75));
    CHECK(!comp(29, 97));
    CHECK(!comp(97, 97));

    CHECK(is_ordered(list[0], comp));
    CHECK(is_ordered(list[1], comp));
    CHECK(is_ordered(list[2], comp));
    CHECK(!is_ordered(list[3], comp));
    CHECK(!is_ordered(list[4], comp));
    CHECK(!is_ordered(list[5], comp));

    CHECK(*middle_if_ordered(list[0], comp) == 61);
    CHECK(*middle_if_ordered(list[1], comp) == 53);
    CHECK(*middle_if_ordered(list[2], comp) == 29);
    CHECK(!middle_if_ordered(list[3], comp));
    CHECK(!middle_if_ordered(list[4], comp));
    CHECK(!middle_if_ordered(list[5], comp));

    CHECK(part_1(test_input_1) == 143);
}

} //namespace day5

