CC=gcc
CFLAGS=-Wall -Wextra -std=c99
DBGFLAGS=-g -O0 -DDEBUG
EXE=expr_tester_executable


#
# Test file build settings
#
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
TESTDIRUP = ../build
TESTOBJS = $(addprefix $(TESTDIRUP)/, $(OBJS))

#
# Test files link settings
#
LINKOBJS := $(wildcard $(TESTDIRUP)/*.o)
LINKOBJS := $(filter-out $(TESTDIRUP)/$(MAINNAME).o, $(LINKOBJS))

LINKDIRUP = ../build
LINKEXE = $(LINKDIRUP)/$(EXE)
DBGOBJS = $(addprefix $(LINKDIRUP)/, $(LINKOBJS))

# Build rules
build-for-tests: $(TESTOBJS)
$(TESTDIRUP)/%.o: %.c
	@$(CC) -c $(CFLAGS) $(DBGFLAGS) -o $@ $<

# Link rules
link-expr-tester: $(LINKEXE)
$(LINKEXE): $(LINKOBJS)
	@$(CC) $(CFLAGS) $(DBGFLAGS) -o $(LINKEXE) $^