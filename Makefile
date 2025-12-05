CXX=clang++
CXXFLAGS=-std=c++23 -Wall -Wextra

all: directory_analyzer

directory_analyzer: src/main.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o directory_analyzer

setup-test-hardlinks:
	@rm -f example_directory/hardlink*.txt
	@ln example_directory/example_file.txt example_directory/hardlink1.txt
	@ln example_directory/example_file.txt example_directory/hardlink2.txt
	@ln example_directory/example_file.txt example_directory/hardlink3.txt
	@echo "Created hardlinks for testing"
	@ls -li example_directory/ | grep example_file

run-example: directory_analyzer setup-test-hardlinks
	./directory_analyzer ./example_directory/example_file.txt example_directory

run-example-subdirectory: directory_analyzer
	./directory_analyzer example_subdirectory example_directory

clean:
	rm -f directory_analyzer
	rm -f example_directory/hardlink*.txt
