#include "paging.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>


using namespace std;

PagingManager::PagingManager(int memSize, int frameSize){
    totalMemorySize = memSize;
    pageSize = frameSize;
    totalFrames = totalMemorySize / pageSize;

    pageTable.clear();
    frameTable.assign(totalFrames,-1);
}

bool PagingManager::allocateProcess(int pid,int processSize){
    int pagesNeeded = (processSize + pageSize -1) / pageSize;

    if(pagesNeeded > count(frameTable.begin(),frameTable.end(), -1)){
        cout << "Not enough frames to allocate process " << pid << "\n";
        return false;
    }

    vector<int> allocatedFrames;
    for(int i = 0;i<totalFrames && pagesNeeded > 0;i++){
        if(frameTable[i] == -1){
            frameTable[i] = pid;
            allocatedFrames.push_back(i);
            pagesNeeded--;
        }
    }

    pageTable[pid] = allocatedFrames;
    cout << "Process " << pid << " allocated using paging.\n";
    return true;

}
void PagingManager::freeProcess(int pid){
    if(pageTable.find(pid) == pageTable.end()){
        cout << "No such process " << pid << " found.\n";
        return ;
    }

    for(int frame:pageTable[pid]){
        frameTable[frame] = -1;
    }

    pageTable.erase(pid);
    cout << "Freed memory of process " << pid << "\n";
}
void PagingManager::printPageTable() const {
    cout << "\nPage Table:\n";
    for (const auto& entry : pageTable) {
        cout << " Process " << entry.first << " → Frames: ";
        for (int frame : entry.second) {
            cout << frame << " ";
        }
        cout << "\n";
    }
    cout << endl;
}

void PagingManager::printFrameTable() const {
    cout << "\n Frame Table:\n";
    for (int i = 0; i < totalFrames; ++i) {
        cout << " Frame " << setw(2) << i << ": ";
        if (frameTable[i] == -1)
            cout << "Free\n";
        else
            cout << "Occupied by P" << frameTable[i] << "\n";
    }
}