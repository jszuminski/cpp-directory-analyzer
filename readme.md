# cpp-directory-analyzer

## Example

```
➜  cpp-directory-analyzer git:(master) make app && make run-example-subdirectory
clang++ -std=c++23 -Wall -Wextra src/main.cpp -o directory_analyzer
./directory_analyzer example_subdirectory example_directory
1) Absolute path:   "/Users/jakubszuminski/Documents/Coding/cpp-directory-analyzer/example_directory/example_subdirectory"
2) File type:       directory
3) Permissions:     rwxr-xr-x
4) Owner:           jakubszuminski
5) I-Node:          101941927
6) Hard link count: 3
```

## Creating example hardlink

```
> cd example_directory
> ln example_file.txt example_file_hardlink.txt
```
