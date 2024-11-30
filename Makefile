CXXFLAGS = -O3 -Wall -Wextra -Werror -std=c++20 -pedantic -pedantic-errors -Wconversion -Wsign-conversion -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2
CHECKFLAGS = --enable=all --std=c++20 --error-exitcode=1 --check-level=exhaustive --suppress=missingIncludeSystem --suppress=checkersReport --suppress=unusedFunction --suppress=unmatchedSuppression

SRC = $(shell find src -type f -name '*.cpp')
OBJS = $(SRC:.cpp=.o)

aoc2024: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

src/main.o: src/main.cpp src/days.hpp src/common.hpp
	cppcheck $(CHECKFLAGS) $<
	$(CXX) $(CXXFLAGS) -c -o $@ $<

src/%.o: src/%.cpp src/common.hpp
	cppcheck $(CHECKFLAGS) $<
	$(CXX) $(CXXFLAGS) -c -o $@ $<


src/days.hpp: $(shell find src/ -type f -name 'day*.cpp')
	echo "#pragma once" > $@
	echo '#include "common.hpp"' >> $@
	for n in $(shell find src/ -name day*.cpp | sed 's|src/day\([0-9]\+\).cpp|\1|'); do echo "DAY($$n);" >> $@; done


clean:
	rm -f src/days.hpp
	rm -f src/*.o
	rm -f aoc2024
