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



bool allocateMemory(vector<MemoryBlock>& memory, int id, int size) {
    for (auto it = memory.begin(); it != memory.end(); ++it) {
        if (!it->allocated && it->size >= size) {
            if (it->size > size) {
                // Split block into allocated + leftover
                MemoryBlock remaining(-1, it->size - size, false);
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
    cout << "❌ Failed to allocate " << size << "KB to Process " << id << " (Not enough space)\n";
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



void runMemoryManager(){
    cout << "\n--- Memory Manager Simulation ---\n";

    vector<MemoryBlock> memory = {
        MemoryBlock(-1 , 100 , false),
        MemoryBlock(-1,100,false),
        MemoryBlock(-1,100,false)
    };

    printMemoryState(memory);

    allocateMemory(memory, 1, 50);
    allocateMemory(memory, 2, 80);
    allocateMemory(memory, 3, 70);
    printMemoryState(memory);


    freeMemory(memory, 2);
    printMemoryState(memory);


    allocateMemory(memory, 4, 60);
    printMemoryState(memory);

    cout << "\n Memory Manager Simulation Complete \n";
}
