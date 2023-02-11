#Makefile for Final Exam Practical Portion
#Sterling LaBarbera spl2q2
#14096504


#src:~/opt/wiringPi/include/
#LINKS := /opt/wiringPi/lib/libwiringPi.so
TARGET := rpi
CFLAGS := -std=c11 -O3 -pthread
CXXFLAGS := -std=c++11 -O3 -pthread

SOURCES := $(wildcard *.cc)
OBJS := $(wildcard *.o)
HEADERS := $(wildcard *.h)

# LDFLAGS
# Extra flags to give to compilers when they are supposed to invoke the linker, 
# ‘ld’, such as -L. Libraries (-lfoo) should be added to the LDLIBS variable instead.

LDFLAGS += -pthread

# LDLIBS
# Library flags or names given to compilers when they are supposed to invoke the linker, ‘ld’.
# LOADLIBES is a deprecated (but still supported) alternative to LDLIBS. 
# Non-library linker flags, such as -L, should go in the LDFLAGS variable.

vpath %.so /opt/wiringPi/lib/
LDLIBS += -lwiringPi

LINK.o = $(LINK.cc)

$(TARGET) : $(OBJS)
	$(LINK.o) $^ $(LDLIBS) -o $@
	
.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
