# Makefile
TARGETS: SegFault_Example gdb-ex1 gdb-ex2 gdb-ex3 gdb-ex4 gdb-ex5 gdb-ex6 gdb-ex7 gdb-ex8
CFLAGS := -std=c11 -ggdb -O0
.PHONY: all
all: 

SegFault_Example:SegFault_Example.c
gdb-ex1:gdb-ex1.c  
gdb-ex2:gdb-ex2.c  
gdb-ex3:gdb-ex3.c  
gdb-ex4:gdb-ex4.c  
gdb-ex5:gdb-ex6.c 
gdb-ex6:gdb-ex5.c 
gdb-ex7:gdb-ex7.c 
gdb-ex8:gdb-ex8.c

.PHONY: clean
clean:
	rm -f *.o $(TARGETS)
