CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./src
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = src/tests

# --- Source Files for the Main Application ---
APP_SRCS = $(SRC_DIR)/main.cpp \
           $(SRC_DIR)/cli/system.cpp \
           $(SRC_DIR)/scheduler/scheduler.cpp \
           $(SRC_DIR)/memory/virtual_memory/virtual_memory.cpp \
           $(SRC_DIR)/core/mutex.cpp

# --- Source Files for Tests ---
VM_TEST_SRCS = $(SRC_DIR)/memory/virtual_memory/virtual_memory.cpp $(TEST_DIR)/vmt.cpp
SCHED_TEST_SRCS = $(SRC_DIR)/scheduler/scheduler.cpp $(TEST_DIR)/test_scheduler.cpp
FS_TEST_SRCS = $(SRC_DIR)/filesystem/filesystem.cpp $(TEST_DIR)/test_filesystem.cpp

# --- Source files for the full integration test ---
INTEGRATION_TEST_SRCS = $(SRC_DIR)/cli/system.cpp \
                        $(SRC_DIR)/core/mutex.cpp \
                        $(SRC_DIR)/scheduler/scheduler.cpp \
                        $(SRC_DIR)/memory/virtual_memory/virtual_memory.cpp \
                        $(TEST_DIR)/test_integration.cpp

# --- Build Rules ---
all: build/main

build/main:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(APP_SRCS)

build/test_vm:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(VM_TEST_SRCS)

build/test_scheduler:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(SCHED_TEST_SRCS)

build/test_fs:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(FS_TEST_SRCS)

# --- NEW: Rule to build the integration test ---
build/test_integration:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(INTEGRATION_TEST_SRCS)

# --- Run Rules ---
run: build/main
	./$(BUILD_DIR)/main

test_vm: build/test_vm
	./$(BUILD_DIR)/test_vm

test_scheduler: build/test_scheduler
	./$(BUILD_DIR)/test_scheduler

test_fs: build/test_fs
	./build/test_fs

# --- Rule to run the integration test ---
test_integration: build/test_integration
	./build/test_integration

# --- Utility ---
clean:
	rm -rf $(BUILD_DIR)