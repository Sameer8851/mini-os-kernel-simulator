CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./src
SRC_DIR = src
BUILD_DIR = build

all: build/main

build/main: \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/scheduler/scheduler.cpp \
    $(SRC_DIR)/memory/memory.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -o build/main \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/scheduler/scheduler.cpp \
    $(SRC_DIR)/memory/memory.cpp

run: build/main
	./build/main

clean:
	rm -rf build
