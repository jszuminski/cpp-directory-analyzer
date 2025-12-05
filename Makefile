CXX=clang++
CXXFLAGS=-std=c++23 -Wall -Wextra

all: directory_analyzer

app: src/main.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o directory_analyzer

run-example: directory_analyzer
	./directory_analyzer ./example_directory/example_file.txt example_directory

run-example-subdirectory: directory_analyzer
	./directory_analyzer example_subdirectory example_directory

clean:
	rm -f directory_analyzer
