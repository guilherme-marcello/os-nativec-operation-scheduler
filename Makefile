# Compiler and linker options
CC      := gcc
CFLAGS  := -Wall -O3 -g
LDFLAGS :=

# Directories
SRCDIR  := src
BINDIR  := bin
OBJDIR := obj

# Source files
SRCS    := $(wildcard $(SRCDIR)/*.c)
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Binary target
TARGET  := $(BINDIR)/main

# Rules
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BINDIR)/*
	rm -rf $(OBJDIR)/*