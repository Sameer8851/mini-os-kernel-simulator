#include "virtual_memory.hpp"
#include <iostream>
#include <queue>
#include <climits>


using namespace std;

VirtualMemoryManager::VirtualMemoryManager(int memorySize, int pageSize, ReplacementPolicy policy)
    : pageSize(pageSize), pageFaults(0),accessCounter(0),policy(policy)
{
    totalFrames = memorySize / pageSize;
    frameTable.resize(totalFrames, {-1, -1});  
}

void VirtualMemoryManager::allocateProcess(int processId, int numPages) {
    if (processPageTables.find(processId) != processPageTables.end()) {
        cout << "Process " << processId << " already exists.\n";
        return;
    }

    PageTable pt;
    for (int i = 0; i < numPages; ++i) {
        pt[i] = PageTableEntry();  // initially invalid
    }

    processPageTables[processId] = pt;
    cout << "Allocated " << numPages << " pages to process " << processId << ".\n";
}

void VirtualMemoryManager::handlePageFault(int processId,int virtualPageNumber,PageTable& pt){
    pageFaults++;

     // Try to find a free frame
    for (int i = 0; i < totalFrames; ++i) {
        if (frameTable[i].first == -1) {
            frameTable[i] = {processId, virtualPageNumber};
            pt[virtualPageNumber].frameNumber = i;
            pt[virtualPageNumber].valid = true;
            pt[virtualPageNumber].lastAccessTime = accessCounter++;
            if (policy == ReplacementPolicy::FIFO) {
                pageQueue.push({processId, virtualPageNumber});
            }
            return;
        }
    }

    // No free frame; replace page based on policy
    if (policy == ReplacementPolicy::FIFO) {
        // Evict oldest page in FIFO queue
        auto toEvict = pageQueue.front();
        pageQueue.pop();

        int victimPid = toEvict.first;
        int victimVpn = toEvict.second;
        int frame = processPageTables[victimPid][victimVpn].frameNumber;

        processPageTables[victimPid][victimVpn].valid = false;
        frameTable[frame] = {processId, virtualPageNumber};

        pt[virtualPageNumber].frameNumber = frame;
        pt[virtualPageNumber].valid = true;
        pt[virtualPageNumber].lastAccessTime = accessCounter++;
        pageQueue.push({processId, virtualPageNumber});
    }
    else if (policy == ReplacementPolicy::LRU) {
        // Find least recently used page
        int lruPid = -1, lruVpn = -1;
        unsigned long minAccessTime = ULONG_MAX;
        int lruFrame = -1;

        for (int i = 0; i < totalFrames; ++i) {
            int pid = frameTable[i].first;
            int vpn = frameTable[i].second;
            unsigned long accessTime = processPageTables[pid][vpn].lastAccessTime;

            if (accessTime < minAccessTime) {
                minAccessTime = accessTime;
                lruPid = pid;
                lruVpn = vpn;
                lruFrame = i;
            }
        }

        // Evict LRU page
        processPageTables[lruPid][lruVpn].valid = false;
        frameTable[lruFrame] = {processId, virtualPageNumber};

        pt[virtualPageNumber].frameNumber = lruFrame;
        pt[virtualPageNumber].valid = true;
        pt[virtualPageNumber].lastAccessTime = accessCounter++;
    }
}


void VirtualMemoryManager::accessPage(int processId, int virtualPageNumber) {
    if (processPageTables.find(processId) == processPageTables.end()) {
        cout << "Process " << processId << " not found.\n";
        return;
    }

    PageTable &pt = processPageTables[processId];

    if (pt.find(virtualPageNumber) == pt.end()) {
        cout << "Virtual page " << virtualPageNumber << " not allocated.\n";
        return;
    }

    if (!pt[virtualPageNumber].valid) {
        cout << "Page fault at P" << processId << " VP" << virtualPageNumber << "\n";
        handlePageFault(processId,virtualPageNumber,pt);
    } else {
        int frame = pt[virtualPageNumber].frameNumber;
        int physicalAddress = frame * pageSize;
        cout << "-> Physical Address: " << physicalAddress << " (Frame " << frame << ")\n";
    }
}


void VirtualMemoryManager::freeProcess(int processId) {
    if (processPageTables.find(processId) == processPageTables.end()) {
        cout << "Process " << processId << " not found.\n";
        return;
    }

    PageTable &pt = processPageTables[processId];

    // Free frames used by this process
    for (auto &entry : pt) {
        if (entry.second.valid) {
            int frame = entry.second.frameNumber;
            frameTable[frame] = {-1, -1};
        }
    }

    // Remove from fifo
    if (policy == ReplacementPolicy::FIFO) {
        queue<pair<int,int>> newQueue;
        while(!pageQueue.empty()){
            auto p = pageQueue.front();
            pageQueue.pop();
            if(p.first != processId){
                newQueue.push(p);
            }
        }
        pageQueue = move(newQueue);
    }

    processPageTables.erase(processId);
    cout << "Freed process " << processId << " and its pages.\n";
}

void VirtualMemoryManager::printPageTable() const {
    cout << "\n=== Page Tables ===\n";
    for (const auto &proc : processPageTables) {
        int pid = proc.first;
        const PageTable &pt = proc.second;

        cout << "Process " << pid << ":\n";
        cout << "Page\tFrame\tValid\tLastAccessTime\n";
        for (const auto &entry : pt) {
            cout << entry.first << "\t"
                 << entry.second.frameNumber << "\t"
                 << (entry.second.valid ? "Yes" : "No") << "\n"
                 << entry.second.lastAccessTime << "\n";
        }
        cout << "\n";
    }
    cout << "Total Page Faults: " << pageFaults << "\n";
}

void VirtualMemoryManager::printFrameTable() const {
    cout << "\n=== Frame Table ===\n";
    cout << "Frame\tProcess\tPage\n";
    for (int i = 0; i < totalFrames; ++i) {
        if (frameTable[i].first == -1) {
            cout << i << "\tFree\t-\n";
        } else {
            cout << i << "\t" << frameTable[i].first
                 << "\t" << frameTable[i].second << "\n";
        }
    }
}
