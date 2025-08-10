CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./src
SRC_DIR = src
BUILD_DIR = build

all: build/main

build/main: \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/scheduler/scheduler.cpp \
    $(SRC_DIR)/memory/memory.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/main \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/scheduler/scheduler.cpp \
    $(SRC_DIR)/memory/memory.cpp \
    $(SRC_DIR)/memory/memory_cli.cpp \
    $(SRC_DIR)/paging/paging.cpp \
    $(SRC_DIR)/paging/paging-cli.cpp \
    $(SRC_DIR)/memory/virtual_memory/virtual_memory.cpp \
    $(SRC_DIR)/memory/virtual_memory/vm_cli.cpp


build/test: \
    $(SRC_DIR)/memory/virtual_memory/virtual_memory.cpp \
    $(SRC_DIR)/memory/virtual_memory/vmt.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/test \
    $(SRC_DIR)/memory/virtual_memory/virtual_memory.cpp \
    $(SRC_DIR)/memory/virtual_memory/vmt.cpp

run: build/main
	./$(BUILD_DIR)/main

test: build/test
	./$(BUILD_DIR)/test

clean:
	rm -rf $(BUILD_DIR)
