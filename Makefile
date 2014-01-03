FLAGS="-std=c++11 -I /home/user/boost_1_53_0 -Wall -Wextra -g"

all: t

test: test.cpp virus_genealogy.hpp
	g++ $(FLAGS) virus_genealogy.hpp test.cpp -o test
t: t01.cc virus_genealogy.hpp
	g++ -std=c++11 -I /home/user/boost_1_53_0 -Wall -Wextra -ggdb3 t01.cc -o t
clean:
	rm -f test t *orig *~
