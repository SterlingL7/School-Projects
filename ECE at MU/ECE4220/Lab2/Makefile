# Makefile
TARGETS := Lab2_Part1
CFLAGS := -std=c11 -ggdb -O0
CC := gcc
LDFLAGS := -lpthread
.PHONY: all
all: Lab2_Part1

Lab2_Part1.o:Lab2_Part1.c
Lab2_Part1:Lab2_Part1.o

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(TARGETS)
