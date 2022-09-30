EXE=ifj22_compiler_executable

DBGDIR=debug_build
BLDDIR=build

.PHONY: all build debug createdirs clean

# Deafult
all: build

# Pomocne 
createdirs:
	@mkdir -p $(DBGDIR) $(BLDDIR)

clean:
	rm -rf build debug_build

clearterminal:
	@clear
	
debug: createdirs
	@cd src && $(MAKE) debug EXE="$(EXE)" DBGDIR="$(DBGDIR)"

build: createdirs
	@cd src && $(MAKE) build EXE="$(EXE)" BLDDIR="$(BLDDIR)"

run: debug
	./$(DBGDIR)/$(EXE)

gdb: debug
	gdb ./$(DBGDIR)/$(EXE)
	
valgrind: debug
	valgrind ./$(DBGDIR)/$(EXE)
