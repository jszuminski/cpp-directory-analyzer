# cpp-directory-analyzer

## POPRAWA: podsumowanie wprowadzonych zmian

### 1. Poprawiono wyswietlanie hardlinkow

Poprawiono wywołanie funkcji `find_other_hardlinks_in_dir` z poprawnymi parametrami

Program wymagal 2 argumentow:

1. Plik, dla ktorego szukamy hardlinkow
2. Katalog, w ktorym program ma przeprowadzic wyszukiwanie

### 2. Mozliwosc testowania hardlinkow

Dodalem do `Makefile` komende `setup-test-hardlinks`, ktora tworzy 3 pliki - `hardlink1.txt`, `hardlink2.txt` i `hardlink3.txt`, ktore umozliwiaja testowanie. Wczesniej mialem hardlinki lokalnie, ale przepadaly one podczas klonowania repo, bo git nie zachowuje hardlinkow.

```bash
make setup-test-hardlinks  # tworzy hardlink1.txt, hardlink2.txt, hardlink3.txt
make run-example           # automatycznie setupuje hardlinki i uruchamia program
```

## Przykład użycia

### Kompilacja i uruchomienie

```bash
make                 # kompilacja programu
make run-example     # automatyczne utworzenie hardlinków i uruchomienie testu
```

### Przykładowy output

```
➜  cpp-directory-analyzer git:(master) make run-example
Created hardlinks for testing
121024122 -rw-r--r--@ 4 jakub.szuminski  staff  26 Nov 22 07:50 example_file.txt
./directory_analyzer ./example_directory/example_file.txt example_directory
1) Absolute path:   /Users/jakub.szuminski/Documents/Coding/personal/cpp-directory-analyzer/example_directory/example_file.txt
2) File type:       regular file
3) Permissions:     rw-r--r--
4) Owner:           jakub.szuminski
5) I-Node:          121024122
6) Hard link count: 4
7) Other hard links in directory:
   - /Users/jakub.szuminski/Documents/Coding/personal/cpp-directory-analyzer/example_directory/hardlink1.txt
   - /Users/jakub.szuminski/Documents/Coding/personal/cpp-directory-analyzer/example_directory/hardlink2.txt
   - /Users/jakub.szuminski/Documents/Coding/personal/cpp-directory-analyzer/example_directory/hardlink3.txt
```

### Manualne tworzenie hardlinków (jeśli potrzebne)

```bash
cd example_directory
ln example_file.txt hardlink1.txt
ln example_file.txt hardlink2.txt
ln example_file.txt hardlink3.txt
```

### Sprawdzanie hardlinków

```bash
ls -li example_directory/  # pokaże ten sam i-node dla wszystkich hardlinków
```
