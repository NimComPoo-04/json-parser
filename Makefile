CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c11 -O0

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
EXE = jp.exe

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(OBJ) $(EXE)
