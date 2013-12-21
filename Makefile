FLAGS="-std=c++11 -I /home/user/boost_1_53_0 -Wall -Wextra -g"

all: t

test: test.cpp virus_genealogy.hpp
	g++ $(FLAGS) virus_genealogy.hpp test.cpp -o test
t: t01.cc
	g++ -std=c++11 -I /home/user/boost_1_53_0 -Wall -Wextra -g t01.cc -o t
clean:
	rm -f test *orig *~
