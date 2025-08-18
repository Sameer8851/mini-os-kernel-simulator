#ifndef VIRTUAL_MEMORY_MANAGER_HPP
#define VIRTUAL_MEMORY_MANAGER_HPP

#include <vector>
#include <unordered_map>
#include <map>
#include <queue>

enum class ReplacementPolicy
{
    FIFO,
    LRU,
    CLOCK,
    OPTIMAL
};

struct PageTableEntry
{
    int frameNumber;
    bool valid;
    bool referenced;
    unsigned long lastAccessTime;

    // Protection bits
    bool can_read;
    bool can_write;
    bool can_execute;

    PageTableEntry() : frameNumber(-1),
                       valid(false),
                       referenced(false),
                       lastAccessTime(0),
                       can_read(false),
                       can_write(false),
                       can_execute(false) {}
};

using PageTable = std::unordered_map<int, PageTableEntry>;

struct PageDirectoryEntry
{
    PageTable *pageTable;
    bool valid;

    PageDirectoryEntry() : pageTable(nullptr), valid(false) {}
};
using PageDirectory = std::unordered_map<int, PageDirectoryEntry>;

enum class AccessType{
    READ,
    WRITE,
    EXECUTE
};


enum LogLevel
{
    NORMAL,
    VERBOSE,
    DEBUG
};
using PageTable = std::unordered_map<int, PageTableEntry>;

class VirtualMemoryManager
{
public:
    VirtualMemoryManager(int memorySize, int pageSize, ReplacementPolicy policy);

    void allocateProcess(int processId);
    void accessPage(int processId, int virtualPageNumber,AccessType type);
    void freeProcess(int processId);

    void printPageTable() const;
    void printFrameTable() const;

    int getPageFaults() const { return pageFaults; }
    void runOptimalSimulation(const std::vector<std::pair<int, int>> &referenceString);
    void setLogLevel(LogLevel level);

    void setPagePermissions(int processId,int virtualPageNumber,bool read,bool write,bool execute);

private:
    int pageSize;
    int totalFrames;
    int pageFaults;
    int clockHand;

    // for Optimal algo
    std::vector<std::pair<int, int>> futureReferences;
    int currentReferenceIndex;

    unsigned long accessCounter; // for tracking LRU
    ReplacementPolicy policy;

    std::vector<std::pair<int, int>> frameTable; // (processId, pageNumber), or (-1, -1) if free
    std::map<int, PageDirectory> processPageDirectories;

    std::queue<std::pair<int, int>> pageQueue; // For FIFO: (processId, pageNumber)

    void handlePageFault(int processId, int virtualPageNumber, PageTable &pt, int pti);

    LogLevel current_log_level = NORMAL;
    void log(LogLevel level, const std::string &message) const;
};

void runVirtualMemoryCLI();

#endif
