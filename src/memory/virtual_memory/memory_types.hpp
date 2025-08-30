#ifndef MEMORY_TYPES_HPP
#define MEMORY_TYPES_HPP

#include <unordered_map>

struct PageTableEntry; // Forward declaration
using PageTable = std::unordered_map<int, PageTableEntry>;

struct PageDirectoryEntry {
    PageTable* pageTable;
    bool valid;
    PageDirectoryEntry() : pageTable(nullptr), valid(false) {}
};

using PageDirectory = std::unordered_map<int, PageDirectoryEntry>;

struct PageTableEntry {
    int frameNumber;
    bool valid;
    bool referenced;
    unsigned long lastAccessTime;
    bool can_read, can_write, can_execute;

    PageTableEntry() : frameNumber(-1), valid(false), referenced(false), lastAccessTime(0),
                       can_read(false), can_write(false), can_execute(false) {}
};

#endif