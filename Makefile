CXX = g++
CXXFLAGS = -Wall -std=c++17
SRC_DIR = src
BUILD_DIR = build

all: $(BUILD_DIR)/main.exe

$(BUILD_DIR)/main.exe: $(SRC_DIR)/main.cpp $(SRC_DIR)/scheduler.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/main.exe $(SRC_DIR)/main.cpp $(SRC_DIR)/scheduler.cpp


run: all
	./$(BUILD_DIR)/main.exe

clean:
	rm -rf $(BUILD_DIR)
