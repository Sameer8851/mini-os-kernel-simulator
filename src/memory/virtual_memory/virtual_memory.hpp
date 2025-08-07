#ifndef VIRTUAL_MEMORY_MANAGER_HPP
#define VIRTUAL_MEMORY_MANAGER_HPP

#include <vector>
#include <unordered_map>
#include <map>

struct PageTableEntry {
    int frameNumber;
    bool valid;
    bool referenced;
    bool modified;

    PageTableEntry() : frameNumber(-1), valid(false), referenced(false), modified(false) {}
};

using PageTable = std::unordered_map<int, PageTableEntry>;

class VirtualMemoryManager {
public:
    VirtualMemoryManager(int memorySize, int pageSize);

    void allocateProcess(int processId, int numPages);
    void accessPage(int processId, int virtualPageNumber);
    void freeProcess(int processId);

    void printPageTable() const;
    void printFrameTable() const;

private:
    int pageSize;
    int totalFrames;
    int pageFaults;

    std::vector<std::pair<int, int>> frameTable; // (processId, pageNumber), or (-1, -1) if free
    std::map<int, PageTable> processPageTables; // processId → page table

    std::vector<std::pair<int, int>> pageQueue; // For FIFO: (processId, pageNumber)
};

void runVirtualMemoryCLI();

#endif
