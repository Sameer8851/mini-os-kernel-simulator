#ifndef VIRTUAL_MEMORY_MANAGER_HPP
#define VIRTUAL_MEMORY_MANAGER_HPP

#include <vector>
#include <unordered_map>
#include <map>
#include <queue>

enum class ReplacementPolicy{
    FIFO,
    LRU
};

struct PageTableEntry {
    int frameNumber;
    bool valid;
    bool referenced;
    bool modified;
    unsigned long lastAccessTime;

    PageTableEntry() : frameNumber(-1), valid(false), referenced(false), modified(false),lastAccessTime(0) {}
};

using PageTable = std::unordered_map<int, PageTableEntry>;

class VirtualMemoryManager {
public:
    VirtualMemoryManager(int memorySize, int pageSize,ReplacementPolicy policy);

    void allocateProcess(int processId, int numPages);
    void accessPage(int processId, int virtualPageNumber);
    void freeProcess(int processId);

    void printPageTable() const;
    void printFrameTable() const;

    int getPageFaults() const {return pageFaults; }

private:
    int pageSize;
    int totalFrames;
    int pageFaults;
    unsigned long accessCounter; // for tracking LRU
    ReplacementPolicy policy;

    std::vector<std::pair<int, int>> frameTable; // (processId, pageNumber), or (-1, -1) if free
    std::map<int, PageTable> processPageTables; // processId → page table

    std::queue<std::pair<int, int>> pageQueue; // For FIFO: (processId, pageNumber)
    void handlePageFault(int processId,int virtualPageNumber,PageTable& pt);
};

void runVirtualMemoryCLI();

#endif
