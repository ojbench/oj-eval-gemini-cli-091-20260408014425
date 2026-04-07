CXX = g++
CXXFLAGS = -O3 -std=c++17

all: code

code: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o code

clean:
	rm -f code
