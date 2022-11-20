CC=gcc
CFLAGS=-Wall -Wextra -std=c99

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

# Debug build settings
DBGDIRUP = ../$(DBGDIR)
DBGEXE = $(DBGDIRUP)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIRUP)/, $(OBJS))
DBGCFLAGS = -g -O0 -DDEBUG

# Test build settings
TESTDIRUP = ../tests/build
TESTOBJS = $(addprefix $(TESTDIRUP)/, $(OBJS))

# build settings
BLDDIRUP = ../$(BLDDIR)
BLDEXE = $(BLDDIRUP)/$(EXE)
BLDOBJS = $(addprefix $(BLDDIRUP)/, $(OBJS))
BLDCFLAGS = -DNDEBUG

.PHONY: all build build-for-tests debug

# Deafult
all: build


# Debug build rules
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	@$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXE) $^

$(DBGDIRUP)/%.o: %.c
	@$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

# test build rules
build-for-tests: $(TESTOBJS)
$(TESTDIRUP)/%.o: %.c
	@$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

# Build rules
build: $(BLDEXE)

$(BLDEXE): $(BLDOBJS)
	@$(CC) $(CFLAGS) $(BLDCFLAGS) -o $(BLDEXE) $^

$(BLDDIRUP)/%.o: %.c
	@$(CC) -c $(CFLAGS) $(BLDCFLAGS) -o $@ $<