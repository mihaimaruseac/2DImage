.PHONY: all clean test doc purge tags

TARGET = ./brad
CC = gcc
CPPFLAGS = -Wall -Wextra -g -O0# -Wno-unused
LDFLAGS = -lm
OBJS = globals.o

all: $(TARGET)

$(TARGET): $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET) *.pgm

purge:
	rm -rf $(OBJS) $(TARGET) tags html *.pgm

test: $(TARGET)
	$(TARGET) a.tga result.pgm 1

doc:
	doxygen

tags:
	ctags *

