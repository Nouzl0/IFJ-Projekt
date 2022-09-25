EXE=ifj22_compiler_executable

DBGDIR=debug_build
BLDDIR=build

.PHONY: all build debug createdirs clean

# Deafult
all: build

createdirs:
	@mkdir -p $(DBGDIR) $(BLDDIR)
	
debug: createdirs
	cd src && $(MAKE) debug EXE="$(EXE)" DBGDIR="$(DBGDIR)"

build: createdirs
	cd src && $(MAKE) build EXE="$(EXE)" BLDDIR="$(BLDDIR)"

run: build
	./build/$(EXE)

gdb: debug
	gdb ./debug_build/$(EXE)

clean:
	rm -rf build debug_build