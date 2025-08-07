#include "virtual_memory.hpp"
#include <iostream>
#include <algorithm>


using namespace std;

VirtualMemoryManager::VirtualMemoryManager(int memorySize, int pageSize)
    : pageSize(pageSize), pageFaults(0)
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
        pageFaults++;

        // Try to find free frame
        for (int i = 0; i < totalFrames; ++i) {
            if (frameTable[i].first == -1) {
                frameTable[i] = {processId, virtualPageNumber};
                pt[virtualPageNumber].frameNumber = i;
                pt[virtualPageNumber].valid = true;
                pageQueue.push_back({processId, virtualPageNumber});
                return;
            }
        }

        // FIFO Replacement
        pair<int, int> toEvict = pageQueue.front();
        pageQueue.erase(pageQueue.begin());

        int victimPid = toEvict.first;
        int victimVpn = toEvict.second;
        int frame = processPageTables[victimPid][victimVpn].frameNumber;

        processPageTables[victimPid][victimVpn].valid = false;
        frameTable[frame] = {processId, virtualPageNumber};

        pt[virtualPageNumber].frameNumber = frame;
        pt[virtualPageNumber].valid = true;

        pageQueue.push_back({processId, virtualPageNumber});
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

    // Remove from pageQueue
    pageQueue.erase(
        remove_if(pageQueue.begin(), pageQueue.end(),
                  [processId](pair<int, int> p) { return p.first == processId; }),
        pageQueue.end()
    );

    processPageTables.erase(processId);
    cout << "Freed process " << processId << " and its pages.\n";
}

void VirtualMemoryManager::printPageTable() const {
    cout << "\n=== Page Tables ===\n";
    for (const auto &proc : processPageTables) {
        int pid = proc.first;
        const PageTable &pt = proc.second;

        cout << "Process " << pid << ":\n";
        cout << "Page\tFrame\tValid\n";
        for (const auto &entry : pt) {
            cout << entry.first << "\t"
                 << entry.second.frameNumber << "\t"
                 << (entry.second.valid ? "Yes" : "No") << "\n";
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
