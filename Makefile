# Hydra software,
# 4. jan. 2020.
#
CC=cc
FLAGS= -pedantic-errors -Wall -Wextra
BIN=./hydra

all:
	$(CC) $(FLAGS) -g -o $(BIN) ./src/main.c 

example: all
	$(BIN) example

finish:
	$(CC) $(FLAGS) -O3 -s -o $(BIN) ./src/main.c

clean:
	rm $(BIN)

.PHONY: testme clean

