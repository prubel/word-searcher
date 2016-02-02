solve: solve.cpp
	g++ -Wall -g -o $@ --std=c++11 $?

clean:
	rm -f solve
