#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <vector>
#include <string>

struct MemoryBlock {
    int id;         // process id
    int size;       // size of the memory block
    bool allocated; // used or not

    MemoryBlock(int id, int size, bool allocated)
        : id(id), size(size), allocated(allocated) {}
};

void runMemoryManager();
void runMemoryCLI();
bool allocateMemory(std::vector<MemoryBlock>& memory, int id, int size,int& internalFrag);
int calculateExternalFragmentation(const std::vector<MemoryBlock>& memory,int requestSize);
void freeMemory(std::vector<MemoryBlock>& memory, int id);
void printMemoryState(const std::vector<MemoryBlock>& memory);


#endif
