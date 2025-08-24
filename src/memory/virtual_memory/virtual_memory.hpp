#ifndef VIRTUAL_MEMORY_MANAGER_HPP
#define VIRTUAL_MEMORY_MANAGER_HPP

#include <vector>
#include <string>
#include <map>
#include <queue>
#include "scheduler/pcb.hpp"
#include "memory/virtual_memory/memory_types.hpp"

// --- Enums ---
enum class ReplacementPolicy { FIFO, LRU, CLOCK };
enum class AccessType { READ, WRITE, EXECUTE };
enum LogLevel { NORMAL, VERBOSE, DEBUG };

const int PAGE_TABLE_SIZE = 1024;

class VirtualMemoryManager {
public:
    VirtualMemoryManager(int memorySize, int pageSize, ReplacementPolicy policy);

    void allocateProcess(ProcessControlBlock& pcb);
    void accessPage(ProcessControlBlock& pcb, int virtualPageNumber, AccessType type);
    void freeProcess(ProcessControlBlock& pcb);
    void setPagePermissions(ProcessControlBlock& pcb, int virtualPageNumber, bool read, bool write, bool execute);
    void printPageTable(const ProcessControlBlock& pcb) const;
    void printFrameTable() const;
    int getPageFaults() const { return pageFaults; }
    void setLogLevel(LogLevel level);
    
    // Helpers for testing
    const std::vector<std::pair<ProcessControlBlock*, int>>& getFrameTable() const { return frameTable; }

private:
    int pageSize;
    int totalFrames;
    int pageFaults;
    int clockHand;
    unsigned long accessCounter;
    ReplacementPolicy policy;
    LogLevel current_log_level = NORMAL;

    std::vector<std::pair<ProcessControlBlock*, int>> frameTable;
    std::queue<std::pair<int, int>> pageQueue;

    void handlePageFault(ProcessControlBlock& pcb, int virtualPageNumber, PageTable& pt, int pti);
    void log(LogLevel level, const std::string& message) const;
};

#endif