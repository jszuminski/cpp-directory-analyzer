CXX=clang++
CXXFLAGS=-std=c++23 -Wall -Wextra

all: app

app: src/main.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o app

run: app
	./app

clean:
	rm -f app