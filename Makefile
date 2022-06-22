LUA_DIR=$(HOME)/local/opt/lua-js
BUILD_TYPE=debug
CC=/usr/bin/cc
CXX=/usr/bin/c++
SHELL=/bin/bash

all: build/$(BUILD_TYPE)
	cd build/$(BUILD_TYPE) && cd $$(pwd -P) && cmake .
	$(MAKE) -C build/$(BUILD_TYPE)

build/$(BUILD_TYPE): FORCE
	mkdir -p build/$(BUILD_TYPE)
	current="$$(pwd -P)" && cd build/$(BUILD_TYPE) && cd "$$(pwd -P)" && cmake \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_VERBOSE_MAKEFILE=On \
		-DCMAKE_C_COMPILER="${CC}" \
		-DCMAKE_CXX_COMPILER="${CXX}" \
		-DCLASP_BUILD_TESTS=On \
		-DLIB_POTASSCO_BUILD_TESTS=On \
		-DCLASP_BUILD_EXAMPLES=On \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=On \
		"$${current}"

# compdb can be installed with pip
compdb: build/$(BUILD_TYPE)
	compdb -p "build/$(BUILD_TYPE)" list -1 > compile_commands.json

%:: build/$(BUILD_TYPE) FORCE
	cd build/$(BUILD_TYPE) && cd $$(pwd -P) && cmake .
	$(MAKE) -C build/$(BUILD_TYPE) $@

test: build/$(BUILD_TYPE)
	cd build/$(BUILD_TYPE) && cd $$(pwd -P) && cmake .
	$(MAKE) -C build/$(BUILD_TYPE)
	$(MAKE) -C build/$(BUILD_TYPE) $@ CTEST_OUTPUT_ON_FAILURE=TRUE

FORCE:

.PHONY: all test FORCE
