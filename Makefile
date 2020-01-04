# Hydra software,
# 4. jan. 2020.
#
CC=gcc
FLAGS= -pedantic-errors -Wall -Wextra -Werror
BIN=./hydra

all:
	$(CC) $(FLAGS) -g -o $(BIN) ./src/main.c 

testme: all
	$(BIN) -h example example.out

finish:
	$(CC) $(FLAGS) -O3 -s -o $(BIN) ./src/main.c

clean:
	rm $(BIN)

.PHONY: testme clean

