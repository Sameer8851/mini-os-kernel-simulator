#include <iostream>
#include <vector>
#include <string>
#include "memory/memory.hpp"

using namespace std;


struct MemoryBlock {
    bool isFree;
    string processName;
    int size;
};

void runMemoryManager(){
    cout << "\n--- Memory Manager Simulation ---" << endl;

    const int blockSize = 100;
    const int totalBlocks = 3;
    vector<MemoryBlock> memory(totalBlocks,{true, "",blockSize});

    vector<pair<string, int>> processList = {
        {"P1",50},{"P2",30},{"P3",70}
    };

    for(auto& p : processList){
        bool allocated = false;
        for(auto& block : memory){
            if(block.isFree && p.second <= block.size){
                block.isFree = false;
                block.processName = p.first;
                cout << "Allocated " << p.first << " (Size " << p.second << ") to block\n";
                allocated = true;
                break;
            }
        }
        if(!allocated){
            cout << "Not enough memory for " << p.first << endl;
        }
    }
    cout << "\nDeallocation memory... \n";
    for(auto& block : memory){
        if(!block.isFree){
            cout<< "Process " << block.processName << " finished. Deallocating block.\n";
            block.isFree =true;
            block.processName = "";
        }
    }
    cout << "\nMemory Manager Simulation Complete\n";
}
