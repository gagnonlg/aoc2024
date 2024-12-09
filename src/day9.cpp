#include <cmath>
#include <cstring>
#include <list>

#include "common.hpp"

namespace day9 {

struct File {
    int64_t id;
    int64_t size;
};

struct Free {
    int64_t size;
};

using Block = std::variant<File, Free>;

bool is_file(const Block& block)
{
    return std::holds_alternative<File>(block);
}

bool is_free(const Block& block)
{
    return std::holds_alternative<Free>(block);
}

void print_block(const Block& v)
{
	if (std::holds_alternative<File>(v)) {
	    const File& f = std::get<File>(v);
	    std::cout << "File{id=" << f.id << ",size="<<f.size<<"}"<<std::endl;
	} 
	if (std::holds_alternative<Free>(v)) {
	    const Free& f = std::get<Free>(v);
	    std::cout << "Free{size="<<f.size<<"}"<<std::endl;
	}
}

template <typename Iterator>
void print_blocks(Iterator begin, Iterator end)
{
    std::cout << ">>>" << std::endl;
    for (auto it = begin; it != end; it++)
	print_block(*it);
    std::cout << "<<<" << std::endl;
}

int64_t blocksize(const Block& b)
{
    return std::visit([](const auto& b_){return b_.size;}, b);
    
}


using Disk = std::list<Block>;

template <typename T>
std::pair<typename std::list<T>::iterator,
	  typename std::list<T>::iterator>
swap_blocks(std::list<T>& list,
	    typename std::list<T>::iterator D,
	    typename std::list<T>::iterator S)
{
    auto S_ = std::next(S);

    if constexpr (std::is_same_v<T, Block>) {
	CHECK(blocksize(*D) == blocksize(*S));
	// std::cout << ">>> BEFORE" << std::endl;
	// for (auto it = list.begin(); it != list.end(); it++) {
	//     if (it == D)
	// 	std::cout << "D:";
	//     if (it == S)
	// 	std::cout << "S:";
	//     print_block(*it);
	// }
    }
    
    // D->(...)->S
    list.splice(D, list, S);
    // S->D->(...)
    // print_block(*D);
    list.splice(S_, list, D);
    // S->(...)->D

    // if constexpr (std::is_same_v<T, Block>) {
    // 	CHECK(blocksize(*D) == blocksize(*S));
    // 	std::cout << ">>> AFTER" << std::endl;
    // 	for (auto it = list.begin(); it != list.end(); it++) {
    // 	    if (it == D)
    // 		std::cout << "D:";
    // 	    if (it == S)
    // 		std::cout << "S:";
    // 	    print_block(*it);
    // 	}
    // }

    
    return std::make_pair(S, D);
}

void print_disk(const Disk& disk)
{
    for (const Block& b : disk) {
	std::string id = is_file(b)? std::to_string(std::get<File>(b).id) : ".";
	std::visit([&](const auto& b_){
	    for (size_t i = 0; ans(i) < b_.size; i++)
		std::cout << id;
	}, b);
    }
    std::cout << std::endl;
}

std::pair<Disk::iterator, Disk::iterator>
split_block(Disk& disk, Disk::iterator it, int64_t size)
{
    CHECK(it != disk.end());
    
    Block& block = *it;

    // std::cout << ">>> SplitA" << std::endl;
    // for (auto it_ = disk.begin(); it_ != disk.end(); it_++) {
    // 	if (it_ == it)
    // 	    std::cout << "TOSPLIT("<<size<<"):";
    // 	print_block(*it_);
    // }


    if (is_free(block)) {
	Free& f = std::get<Free>(block);
	CHECK(size < f.size);
	Free newf = Free {f.size - size };
	f.size = size;
	disk.insert(std::next(it), newf);
    } else if (is_file(block)) {
	File& f = std::get<File>(block);
	CHECK(size < f.size);
	File newf = File { f.id, size };
	f.size -= size;
	disk.insert(std::next(it), newf);
    } else {
	throw std::runtime_error("impossible");
    }

    // std::cout << ">>> SplitB" << std::endl;
    // for (auto it_ = disk.begin(); it_ != disk.end(); it_++) {
    // 	if (it_ == it)
    // 	    std::cout << "P:";
    // 	if (it_ == std::next(it))
    // 	    std::cout << "P':";
    // 	print_block(*it_);
    // }


    return std::make_pair(it, std::next(it));
    
}

std::pair<Disk::iterator, Disk::iterator>
fill_free(Disk& disk, Disk::iterator D, Disk::iterator S)
{
    int64_t ssize = blocksize(*S);
    int64_t dsize = blocksize(*D);

    if (dsize == ssize) {
	return swap_blocks(disk, D, S);
    }

    else if (dsize < ssize) {
	// std::cout << dsize << " " << ssize << std::endl;
	auto [s0, s1] = split_block(disk, S, dsize);
	// std::cout << std::distance(disk.rbegin(), s0) << std::endl;
	// std::cout << std::distance(disk.rbegin(), s1) << std::endl;
	// std::cout << std::distance(disk.begin(), D) << std::endl;
	// print_block(*s0);
	// print_block(*s1);
	// print_block(*D);
	return swap_blocks(disk, D, s1);
    }

    else { // if (dsize > ssize) {
	auto [d0, d1] = split_block(disk, D, ssize);
	return swap_blocks(disk, d0, S);
    }

    throw std::logic_error("unreachable");
}

void defrag(Disk& disk, bool trace = false)
{
    // Disk::iterator D = std::find(disk.begin(), disk.end(), [](is_free);
    // Disk::reverse_iterator S = std::find(disk.rbegin(), disk.rend(), is_file);
    auto D = disk.begin();
    auto S = disk.rbegin();

    if (trace) {
	print_disk(disk);
	// print_blocks(disk.begin(), disk.end());
    }
    
    while (true) {
	if (D == std::next(S).base())
	    break;
	if (D == disk.end())
	    break;
	if (!is_free(*D)) {
	    D = std::next(D);
	    continue;
	}
	if (S == disk.rend())
	    break;
	if (!is_file(*S)) {
	    S = std::next(S);
	    continue;
	}
	
	auto its = fill_free(disk, D, std::next(S).base());
	if (trace) {
	    print_disk(disk);
	    // print_blocks(disk.begin(), disk.end());
	}
	D = its.first;
	S = std::prev(std::make_reverse_iterator(its.second));

	D = std::next(D);
	if (D == disk.end() || D == std::next(S).base())
	    break;

	S = std::next(S);
	if (S == disk.rend() || D == std::next(S).base())
	    break;
    }
}
    
int64_t checksum(const Disk& disk)
{
    int64_t acc = 0;
    int64_t i = 0;
    for (const Block& b : disk) {
	if (!is_file(b))
	    break;	    
	int64_t id = std::get<File>(b).id;
	int64_t size = std::get<File>(b).size;
	while (size --> 0) {
	    // std::cout << acc << "+=" << i << "*" << id << std::endl;
	    acc += (i++) * id;
	}
    }
    return acc;
}

using InputData = Disk;

InputData read_input_data(const std::string& input)
{
    InputData data;
    int64_t id = 0;
    bool expect_file = true;
    for (char c : input) {
	int ic = c - '0';
	data.emplace_back(expect_file? Block(File{id++, ic}) : Block(Free{ic}));
	expect_file = !expect_file;
    }
    return data;
}

Answer part_1(const std::string& input)
{
    InputData data = read_input_data(input);
    defrag(data);
    return checksum(data);
}

Answer part_2(const std::string& /*input*/)
{
    throw NotImplemented();
}

void tests()
{
    std::vector<const char *> test_input = {
	"2333133121414131402",
	"1234",
	"12345",
    };

    std::vector<InputData> test_data;
    std::transform(test_input.begin(), test_input.end(),
		   std::back_inserter(test_data), read_input_data);

    // CHECK(test_data.size() == 2);
    CHECK(test_data[0].size() == std::strlen(test_input[0]));
    CHECK(test_data[1].size() == std::strlen(test_input[1]));
    CHECK(std::count_if(test_data[0].begin(), test_data[0].end(), is_file) == 10);
    CHECK(std::count_if(test_data[1].begin(), test_data[1].end(), is_file) == 2);
    CHECK(std::count_if(test_data[0].begin(), test_data[0].end(), is_free) == 9);
    CHECK(std::count_if(test_data[1].begin(), test_data[1].end(), is_free) == 2);

    CHECK(is_file(test_data[0].front())
	  && std::get<File>(test_data[0].front()).id == 0
	  && std::get<File>(test_data[0].front()).size == 2);
    CHECK(is_free(*std::next(test_data[0].begin()))
	  && std::get<Free>(*std::next(test_data[0].begin())).size == 3);
    CHECK(is_file(*std::next(std::next(test_data[0].begin())))
	  && std::get<File>(*std::next(std::next(test_data[0].begin()))).id == 1
	  && std::get<File>(*std::next(std::next(test_data[0].begin()))).size == 3);

    // // 022111222......
    // std::vector<Block> copy = test_data[1];
    // defrag(copy);
    // CHECK(copy.size() == test_data[1].size());
    // auto it = std::find(copy.begin(), copy.end(), is_free);
    // CHECK(it != copy.end());
    // CHECK(std::distance(it, copy.begin()) == 9);
    // CHECK(std::all_of(it, copy.end(), is_free));

    std::list<int> testlist = {1,2,3};
    auto [it0, it1] = swap_blocks(testlist, testlist.begin(), std::prev(testlist.end()));
    CHECK(std::distance(testlist.begin(), testlist.end()) == 3);
    CHECK(it0 != testlist.end());
    CHECK(*it0 == 3);
    CHECK(it1 != testlist.end());
    CHECK(*it1 == 1);

    std::list<Block> copy(test_data[1]);
    auto it = std::next(copy.begin());
    CHECK(is_free(*it));
    auto size = std::get<Free>(*it).size;
    CHECK(std::distance(copy.begin(), copy.end()) == 4);
    auto [it2, it3] = split_block(copy, it, 1);
    CHECK(std::distance(copy.begin(), copy.end()) == 5);
    CHECK(it2 == it);
    CHECK(std::next(it2) == it3);
    CHECK(size == std::get<Free>(*it2).size +  std::get<Free>(*it3).size);

    // copy = std::list<Block>(test_data[1]);
    // auto rit = std::next(copy.rbegin());
    // CHECK(is_file(*rit));
    // size = std::get<File>(*rit).size;
    // auto id = std::get<File>(*rit).id;
    // CHECK(std::distance(copy.begin(), copy.end()) == 4);
    // auto [rit2, rit3] = split_block(copy, rit, 1);
    // CHECK(std::distance(copy.begin(), copy.end()) == 5);
    // CHECK(it2 == it);
    // CHECK(std::prev(rit2) == rit3);
    // CHECK(is_file(*rit2));
    // CHECK(is_file(*rit3));
    // CHECK(size == std::get<File>(*rit2).size +  std::get<File>(*rit3).size);
    // CHECK(id == std::get<File>(*rit2).id);
    // CHECK(id == std::get<File>(*rit3).id);

    copy = test_data[1];
    defrag(copy);
    CHECK(std::is_partitioned(copy.begin(), copy.end(), is_file));

    copy = test_data[0];
    defrag(copy, false);
    CHECK(std::is_partitioned(copy.begin(), copy.end(), is_file));
    CHECK(checksum(copy) == 1928);

    CHECK(part_1(test_input[0]) == 1928);
}
} //namespace day9

