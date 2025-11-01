CXX=clang++
CXXFLAGS=-std=c++23 -Wall -Wextra

all: directory_analyzer

app: src/main.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o directory_analyzer

run: directory_analyzer
	./directory_analyzer

clean:
	rm -f directory_analyzer
