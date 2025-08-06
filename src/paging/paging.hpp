#ifndef PAGING_HPP
#define PAGING_HPP

#include <vector>
#include <unordered_map>

class PagingManager {
private:
    int totalMemorySize;
    int pageSize;
    int totalFrames;

    std::unordered_map<int, std::vector<int>> pageTable;
    std::vector<int> frameTable;                          

public:
    PagingManager(int memorySize, int pageSize);
    
    bool allocateProcess(int pid, int size);
    void freeProcess(int pid);

    void printPageTable() const;
    void printFrameTable() const;
    
    int translateAddress(int pid,int logicalAddress);
    
};

void runPagingCLI();


#endif // PAGING_HPP
