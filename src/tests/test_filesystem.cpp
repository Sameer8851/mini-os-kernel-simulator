#include "filesystem/filesystem.hpp"
#include <iostream>
#include <string>
#include <map>

// Helper for our test
void ASSERT_TRUE(bool condition, const std::string& message) {
    if (condition) {
        std::cout << "[ \033[32mPASS\033[0m ] " << message << std::endl;
    } else {
        std::cout << "[ \033[31mFAIL\033[0m ] " << message << std::endl;
        exit(1);
    }
}

// --- Test Suites ---

void testFileCreateWriteRead(FileSystem& fs) {
    std::cout << "\n--- Testing Create, Write, Read ---\n";
    std::string filename = "hello.txt";
    int inode_num = fs.create(filename);
    
    std::string original_data = "Hello, world!";
    fs.write(inode_num, original_data);

    std::string read_data = fs.read(inode_num);
    ASSERT_TRUE(read_data == original_data, "Data read from file should match data written.");
}

void testFileRemoval(FileSystem& fs) {
    std::cout << "\n--- Testing File Removal ---\n";
    std::string filename = "to_delete.txt";
    int inode_num = fs.create(filename);
    fs.write(inode_num, "some data");

    // Verify it exists before deleting
    ASSERT_TRUE(fs.getRootDirectory().count(filename) == 1, "File should exist before removal.");
    
    // Remove the file
    fs.remove(filename);
    std::cout << "Removed file '" << filename << "'.\n";

    // Verify it's gone from the directory and inode table
    ASSERT_TRUE(fs.getRootDirectory().count(filename) == 0, "File should not exist in directory after removal.");
    ASSERT_TRUE(fs.getInodeTable().count(inode_num) == 0, "Inode should be freed after removal.");
}

// --- Test Runner Main Function ---

int main() {
    std::cout << "===== Running File System Tests =====\n";
    FileSystem fs(100);
    fs.setLogLevel(VERBOSE);

    testFileCreateWriteRead(fs);
    testFileRemoval(fs);

    std::cout << "\n===== All File System Tests Passed! =====\n";
    return 0;
}