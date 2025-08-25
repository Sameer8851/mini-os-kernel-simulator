CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./src
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = src/tests

# Rule to build the main simulator program
all: build/main

build/main:
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

# Rule to build the virtual memory test runner
build/test_vm:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/test_vm \
	$(SRC_DIR)/memory/virtual_memory/virtual_memory.cpp \
	$(TEST_DIR)/vmt.cpp

# --- CORRECTED RULE ---
# Rule to build the scheduler test runner
build/test_scheduler:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/test_scheduler \
	$(SRC_DIR)/scheduler/scheduler.cpp \
	$(TEST_DIR)/test_scheduler.cpp

# Rule to run the main simulator
run: build/main
	./$(BUILD_DIR)/main

# Rule to run the virtual memory tests
test_vm: build/test_vm
	./$(BUILD_DIR)/test_vm

# Rule to run the scheduler tests
test_scheduler: build/test_scheduler
	./$(BUILD_DIR)/test_scheduler

# Rule to clean the build directory
clean:
	rm -rf $(BUILD_DIR)