CC = gcc
CFLAGS = -pthread -g -Wall -Wextra -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L -I../external/include
LINKER = -L../external -lvoids64 -lm

SRC = $(wildcard *.c)
HDR = $(wildcard *.h)

EXE = ../simbly

$(EXE): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LINKER)

.PHONY: clean

clean:
	@rm -f $(EXE)
