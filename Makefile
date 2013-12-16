all: virgen.o

virgen.o: virus_genealogy.hpp
	g++ virus_genealogy.hpp -std=c++11 -o virgen.o