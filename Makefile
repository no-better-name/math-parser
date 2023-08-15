CC = gcc
CCDBGFLAGS = -g3 -ggdb -gdwarf
CCWARNFLAGS = -Wall -Wextra -Wpedantic -fanalyzer
CCFLAGS = $(CCWARNFLAGS) $(CCDBGFLAGS)

.PHONY: all build rebuild clean lexer parser calc

all: lexer parser calc

clean:
	rm -f build/*.so build/*.o build/*.a bin/*

rebuild: clean all

lexer: bin/lexer-test

parser: bin/parser-test

calc: bin/calc-test

bin/lexer-test: build/lexer-test.o build/token.o build/string_utils.o
	$(CC) $(CCFLAGS) build/lexer-test.o build/token.o build/string_utils.o -o bin/lexer-test

bin/parser-test: build/parser-test.o build/token.o build/string_utils.o build/parse.o
	$(CC) $(CCFLAGS) build/parser-test.o build/token.o build/string_utils.o build/parse.o -o bin/parser-test

bin/calc-test: build/calc-test.o build/token.o build/string_utils.o build/parse.o build/calc.o
	$(CC) $(CCFLAGS) build/calc-test.o build/token.o build/string_utils.o build/parse.o build/calc.o -o bin/calc-test -lm

build/lexer-test.o: src/lexer-test.c
	$(CC) $(CCFLAGS) -c src/lexer-test.c -o build/lexer-test.o

build/parser-test.o: src/parser-test.c
	$(CC) $(CCFLAGS) -c src/parser-test.c -o build/parser-test.o

build/calc-test.o: src/calc-test.c
	$(CC) $(CCFLAGS) -c src/calc-test.c -o build/calc-test.o

build/token.o: src/token.c
	$(CC) $(CCFLAGS) -c src/token.c -o build/token.o

build/string_utils.o: src/string_utils.c
	$(CC) $(CCFLAGS) -c src/string_utils.c -o build/string_utils.o

build/parse.o: src/parse.c
	$(CC) $(CCFLAGS) -c src/parse.c -o build/parse.o

build/calc.o: src/calc.c
	$(CC) $(CCFLAGS) -c src/calc.c -o build/calc.o
