#include <iostream>
#include <vector>
#include "memory.hpp"

using namespace std;

void printMemoryState(const vector<MemoryBlock>& memory) {
    cout << "\n Memory State:\n";
    for (const auto& block : memory) {
        cout << "  Block ID: " << (block.id == -1 ? "FREE" : to_string(block.id))
             << " | Size: " << block.size
             << " | Status: " << (block.allocated ? "Allocated" : "Free") << "\n";
    }
    cout << endl;
}

int totalInternalFragmentation = 0;

bool allocateMemory(vector<MemoryBlock>& memory, int id, int size, int& totalInternalFragmentation) {
    
    for (auto it = memory.begin(); it != memory.end(); ++it) {
        if (!it->allocated && it->size >= size) {
            if (it->size > size) {
                int leftover = it->size - size;
                totalInternalFragmentation += leftover;

                // split block
                MemoryBlock remaining(-1, leftover, false);
                it = memory.insert(it + 1, remaining);
                it--;
                it->size = size;
            }
            it->allocated = true;
            it->id = id;

            cout << " Allocated " << size << "KB to Process " << id << "\n";
            return true;
        }
    }
    cout << " Failed to allocate " << size << "KB to Process " << id << " (Not enough space)\n";
    return false;
}

void mergeFreeBlocks(vector<MemoryBlock>& memory){
    for(size_t i = 0;i + 1<memory.size();){
        if(!memory[i].allocated && !memory[i+1].allocated){
            memory[i].size += memory[i+1].size;
            memory.erase(memory.begin()+i+1);
        } else{
            ++i;
        }
    }
}

void freeMemory(vector<MemoryBlock>& memory,int id){
    for(auto& block : memory){
        if (block.allocated && block.id == id){
            block.allocated = false;
            block.id = -1;
            cout << "Freed memory block from Process " << id << "\n";
            mergeFreeBlocks(memory);
            return;
        }
    }
    cout << "No block found for Process " << id << "\n";
}

int calculateExternalFragmentation(const vector<MemoryBlock>& memory, int requestedSize) {
    int totalFree = 0;
    bool hasFittingBlock = false;

    for (const auto& block : memory) {
        if (!block.allocated) {
            totalFree += block.size;
            if (block.size >= requestedSize) {
                hasFittingBlock = true;
            }
        }
    }

    if (!hasFittingBlock && totalFree >= requestedSize) {
        return totalFree;
    }

    return 0;
}


void runMemoryManager() {
    cout << "\n--- Memory Manager: Merging Test ---\n";
    int totalInternalFrag = 0;

    vector<MemoryBlock> memory = {
        MemoryBlock(-1 , 100 , false),
        MemoryBlock(-1 , 100 , false),
        MemoryBlock(-1 , 100 , false)
    };

    printMemoryState(memory);

    allocateMemory(memory, 1, 50,totalInternalFrag);   // Allocates part of block 0
    allocateMemory(memory, 2, 80,totalInternalFrag);   // Allocates part of block 1
    allocateMemory(memory, 3, 90,totalInternalFrag);   // Allocates part of block 2

    printMemoryState(memory);

    freeMemory(memory, 1); // Free block 1
    freeMemory(memory, 2); // Free block 2 — should merge with 1
    freeMemory(memory, 3); // Free block 3 — should merge with merged 1 & 2

    printMemoryState(memory); // Should see 1 large free block
    int frag = calculateExternalFragmentation(memory,60);
    if(frag > 0){
        cout << "External Fragmentation: " << frag << "KB (Requested: 60KB)\n";
    }

    cout << "\n Merging Simulation Complete \n";
}

