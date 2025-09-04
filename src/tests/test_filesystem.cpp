#include "filesystem/filesystem.hpp"
#include <iostream>
#include <string>

// Helper for our test
void ASSERT_EQUALS(const std::string& actual, const std::string& expected, const std::string& message) {
    if (actual == expected) {
        std::cout << "[ \033[32mPASS\033[0m ] " << message << std::endl;
    } else {
        std::cout << "[ \033[31mFAIL\033[0m ] " << message << std::endl;
        std::cout << "  Expected: " << expected << std::endl;
        std::cout << "  Actual:   " << actual << std::endl;
        exit(1);
    }
}

int main() {
    std::cout << "===== Running File System Test =====\n";

    // 1. Initialize the file system with 100 blocks
    FileSystem fs(100);
    fs.setLogLevel(VERBOSE);

    // 2. Create a new file
    std::string filename = "hello.txt";
    int inode_num = fs.create(filename);
    
    // 3. Write data to the file
    std::string original_data = "Hello, world! This is a test of the simple file system.";
    fs.write(inode_num, original_data);

    // 4. Read the data back
    std::string read_data = fs.read(inode_num);

    // 5. Verify the data is correct
    ASSERT_EQUALS(read_data, original_data, "Data read from file should match data written.");

    std::cout << "\n===== File System Test Passed! =====\n";
    return 0;
}