test:
	gcc-13 ValueTest.cpp -std=c++20 -O2 -lstdc++ -Wall -pedantic
	./a.out
	rm a.out
