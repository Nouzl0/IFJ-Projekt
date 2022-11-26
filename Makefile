MAINNAME=ifj22_compiler
EXE=$(MAINNAME)_executable

DBGDIR=debug_build
BLDDIR=build

.PHONY: all build debug createdirs clean

# Deafult
all: build

# Pomocne 
createdirs:
	@mkdir -p $(DBGDIR) $(BLDDIR) ./tests/build

clean:
	rm -rf build debug_build ./tests/build ./tests/lex/*.out ./tests/expr/*.out

clearterminal:
	@clear
	
debug: createdirs
	@cd src && $(MAKE) -s debug EXE="$(EXE)" DBGDIR="$(DBGDIR)"

build: createdirs
	@cd src && $(MAKE) -s build EXE="$(EXE)" BLDDIR="$(BLDDIR)"

run: debug
	./$(DBGDIR)/$(EXE)

gdb: debug
	gdb ./$(DBGDIR)/$(EXE)
	
valgrind: debug
	valgrind ./$(DBGDIR)/$(EXE)
	
valgrindf: debug
	valgrind --leak-check=full ./$(DBGDIR)/$(EXE)

clean-test-build:
	rm -rf ./tests/build

test-lex-build: clean-test-build createdirs
	@cd src && $(MAKE) -s build-for-tests
	@cd tests/lex && $(MAKE) -s build-for-tests
	@cd tests/lex && $(MAKE) -s link-lex-tester MAINNAME="$(MAINNAME)"

test-lex: test-lex-build
	@cd tests/build && ./lex_tester_executable

test-lex-valgrind: test-lex-build
	@cd tests/build && valgrind --leak-check=full ./lex_tester_executable
	

test-expr-build: clean-test-build createdirs
	@cd src && $(MAKE) -s build-for-tests
	@cd tests/expr && $(MAKE) -s build-for-tests
	@cd tests/expr && $(MAKE) -s link-expr-tester MAINNAME="$(MAINNAME)"

test-expr: test-expr-build
	@cd tests/build && ./expr_tester_executable

test-expr-valgrind: test-expr-build
	@cd tests/build && valgrind --leak-check=full  ./expr_tester_executable
	
test:
	make -s test-lex
	make -s test-expr
	