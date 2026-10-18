CXX      := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude -MMD -MP
SRC      := $(wildcard src/*.cpp)
OBJ      := $(patsubst src/%.cpp,build/%.o,$(SRC))
DEP      := $(OBJ:.o=.d)

.PHONY: all test demo clean

all: build/barcode_tests build/barcode_demo

build:
	mkdir -p build

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/barcode_tests: tests/state_machine_test.cpp $(OBJ) | build
	$(CXX) $(CXXFLAGS) $^ -o $@

build/barcode_demo: demo/main.cpp $(OBJ) | build
	$(CXX) $(CXXFLAGS) $^ -o $@

test: build/barcode_tests
	./build/barcode_tests

demo: build/barcode_demo
	./build/barcode_demo

clean:
	rm -rf build

-include $(DEP)
