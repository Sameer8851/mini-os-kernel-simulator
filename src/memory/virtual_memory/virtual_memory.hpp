#ifndef VIRTUAL_MEMORY_MANAGER_HPP
#define VIRTUAL_MEMORY_MANAGER_HPP

#include <vector>
#include <unordered_map>
#include <map>
#include <queue>

enum class ReplacementPolicy{
    FIFO,
    LRU,
    CLOCK,
    OPTIMAL
};

struct PageTableEntry {
    int frameNumber;
    bool valid;
    bool referenced;
    bool modified;
    unsigned long lastAccessTime;

    PageTableEntry() : frameNumber(-1), valid(false), referenced(false), modified(false),lastAccessTime(0) {}
};
enum LogLevel {NORMAL,VERBOSE,DEBUG};
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
    void runOptimalSimulation(const std::vector<std::pair<int, int>>& referenceString);
    void setLogLevel(LogLevel level);
    

private:
    int pageSize;
    int totalFrames;
    int pageFaults;
    int clockHand;

    // for Optimal algo
    std::vector<std::pair<int,int>> futureReferences;
    int currentReferenceIndex;

    unsigned long accessCounter; // for tracking LRU
    ReplacementPolicy policy;

    std::vector<std::pair<int, int>> frameTable; // (processId, pageNumber), or (-1, -1) if free
    std::map<int, PageTable> processPageTables; // processId → page table

    std::queue<std::pair<int, int>> pageQueue; // For FIFO: (processId, pageNumber)
    void handlePageFault(int processId,int virtualPageNumber,PageTable& pt);

    LogLevel current_log_level = NORMAL;
    void log(LogLevel level,const std::string& message) const;

    
};

void runVirtualMemoryCLI();


#endif
