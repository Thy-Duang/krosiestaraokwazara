all: test

test: test.cpp virus_genealogy.hpp
	g++ virus_genealogy.hpp test.cpp -o test -std=c++11

clean:
	rm -f test *orig *~
