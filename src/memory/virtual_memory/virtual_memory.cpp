#include "virtual_memory.hpp"
#include <iostream>
#include <queue>
#include <climits>
#include <sstream> // Include this for stringstream

using namespace std;

VirtualMemoryManager::VirtualMemoryManager(int memorySize, int pageSize, ReplacementPolicy policy)
    : pageSize(pageSize), pageFaults(0),accessCounter(0),policy(policy),clockHand(0), currentReferenceIndex(0)
{
    totalFrames = memorySize / pageSize;
    frameTable.resize(totalFrames, {-1, -1});
}

void VirtualMemoryManager::log(LogLevel level, const string& message) const {
    if (current_log_level >= level) {
        cout << message << endl;
    }
}

void VirtualMemoryManager::setLogLevel(LogLevel level){
    current_log_level = level;
}

void VirtualMemoryManager::allocateProcess(int processId, int numPages) {
    stringstream ss;
    if (processPageTables.find(processId) != processPageTables.end()) {
        ss << "Error: Process " << processId << " already exists.";
        log(NORMAL, ss.str());
        return;
    }
    
    PageTable pt;
    for (int i = 0; i < numPages; ++i) {
        pt[i] = PageTableEntry(); // initially invalid
    }

    processPageTables[processId] = pt;
    ss << "Allocated " << numPages << " pages to process " << processId << ".";
    log(NORMAL, ss.str());
}

void VirtualMemoryManager::handlePageFault(int processId, int virtualPageNumber, PageTable& pt) {
    pageFaults++;
    stringstream ss;
    ss << "Handling page fault...";
    log(VERBOSE, ss.str());

    // Try to find a free frame first
    for (int i = 0; i < totalFrames; ++i) {
        if (frameTable[i].first == -1) {
            stringstream ss_found;
            ss_found << "Found free frame " << i << ".";
            log(VERBOSE, ss_found.str());
            frameTable[i] = {processId, virtualPageNumber};
            pt[virtualPageNumber].frameNumber = i;
            pt[virtualPageNumber].valid = true;
            pt[virtualPageNumber].lastAccessTime = accessCounter++;
            pt[virtualPageNumber].referenced = true; // For Clock
            if (policy == ReplacementPolicy::FIFO) {
                pageQueue.push({processId, virtualPageNumber});
            }
            return;
        }
    }

    // If no free frame, begin replacement logic
    log(VERBOSE, "No free frames. Starting replacement...");
    int victimFrame = -1; // This will be set by the policy block

    if (policy == ReplacementPolicy::FIFO) {
        auto toEvict = pageQueue.front();
        pageQueue.pop();
        victimFrame = processPageTables.at(toEvict.first).at(toEvict.second).frameNumber;
    }
    else if (policy == ReplacementPolicy::LRU) {
        unsigned long minAccessTime = ULONG_MAX;
        for (int i = 0; i < totalFrames; ++i) {
            int pid = frameTable[i].first;
            int vpn = frameTable[i].second;
            if (processPageTables.at(pid).at(vpn).lastAccessTime < minAccessTime) {
                minAccessTime = processPageTables.at(pid).at(vpn).lastAccessTime;
                victimFrame = i;
            }
        }
    }
    else if (policy == ReplacementPolicy::CLOCK) {
        while (true) {
            int pid_at_hand = frameTable[clockHand].first;
            int vpn_at_hand = frameTable[clockHand].second;
            if (processPageTables.at(pid_at_hand).at(vpn_at_hand).referenced == false) {
                victimFrame = clockHand;
                clockHand = (clockHand + 1) % totalFrames; // Advance hand for next time
                break;
            } else {
                processPageTables.at(pid_at_hand).at(vpn_at_hand).referenced = false;
                clockHand = (clockHand + 1) % totalFrames;
            }
        }
    }
    else if (policy == ReplacementPolicy::OPTIMAL) {
        int farthest = -1;
        int frame_to_evict = -1;
        for (int i = 0; i < totalFrames; ++i) {
            int current_pid = frameTable[i].first;
            int current_vpn = frameTable[i].second;
            int distance = 0;
            bool found = false;
            for (size_t j = currentReferenceIndex + 1; j < futureReferences.size(); ++j) {
                if (futureReferences[j].first == current_pid && futureReferences[j].second == current_vpn) {
                    distance = j;
                    found = true;
                    break;
                }
            }
            if (!found) {
                frame_to_evict = i;
                break;
            }
            if (distance > farthest) {
                farthest = distance;
                frame_to_evict = i;
            }
        }
        victimFrame = frame_to_evict;
    }

    // --- Common eviction logic for ALL replacement policies ---
    if (victimFrame != -1) {
        // 1. Identify and invalidate the victim's page table entry
        int victimPid = frameTable[victimFrame].first;
        int victimVpn = frameTable[victimFrame].second;
        stringstream ss_evict;
        ss_evict << "Evicting P" << victimPid << " VP" << victimVpn << " from frame " << victimFrame << ".";
        log(VERBOSE, ss_evict.str());
        
        processPageTables.at(victimPid).at(victimVpn).valid = false;
        processPageTables.at(victimPid).at(victimVpn).frameNumber = -1;

        // 2. Load the new page into the newly freed frame
        frameTable[victimFrame] = {processId, virtualPageNumber};
        pt[virtualPageNumber].frameNumber = victimFrame;
        pt[virtualPageNumber].valid = true;
        pt[virtualPageNumber].lastAccessTime = accessCounter++;
        pt[virtualPageNumber].referenced = true;

        // 3. Update FIFO queue if necessary
        if (policy == ReplacementPolicy::FIFO) {
            pageQueue.push({processId, virtualPageNumber});
        }
    } else {
        // This safety check prevents a crash if a policy fails to find a victim
        log(NORMAL, "CRITICAL ERROR: Could not determine a victim frame!");
    }
}

void VirtualMemoryManager::accessPage(int processId, int virtualPageNumber) {
    stringstream ss;
    if (processPageTables.find(processId) == processPageTables.end()) {
        ss << "Error: Process " << processId << " not found.";
        log(NORMAL, ss.str());
        return;
    }

    PageTable &pt = processPageTables.at(processId);

    if (pt.find(virtualPageNumber) == pt.end()) {
        ss << "Virtual page " << virtualPageNumber << " not allocated for process " << processId << ".";
        log(NORMAL, ss.str());
        return;
    }

    if (!pt.at(virtualPageNumber).valid) {
        ss << "Page fault at P" << processId << " VP " << virtualPageNumber;
        log(VERBOSE, ss.str());
        handlePageFault(processId, virtualPageNumber, pt);
    } else {
        ss << "Page access successful for P" << processId << " VP " << virtualPageNumber << ".";
        log(VERBOSE, ss.str());
        
        pt.at(virtualPageNumber).lastAccessTime = accessCounter++;
        pt.at(virtualPageNumber).referenced = true;

        int frame = pt.at(virtualPageNumber).frameNumber;
        int physicalAddress = frame * pageSize;
        stringstream ss_pa;
        ss_pa << "-> Physical Address: " << physicalAddress << " (Frame " << frame << ")";
        log(DEBUG, ss_pa.str());
    }
}

void VirtualMemoryManager::freeProcess(int processId) {
    stringstream ss;
    if (processPageTables.find(processId) == processPageTables.end()) {
        ss << "Error: Process " << processId << " not found.";
        log(NORMAL, ss.str());
        return;
    }

    PageTable& pt = processPageTables.at(processId);
    for (const auto& entry_pair : pt) {
        const PageTableEntry& page_entry = entry_pair.second;
        
        if (page_entry.valid) {
            int frame_to_free = page_entry.frameNumber;
            if (frame_to_free >= 0 && frame_to_free < totalFrames) {
                frameTable[frame_to_free] = {-1, -1};
            }
        }
    }

    if (policy == ReplacementPolicy::FIFO) {
        queue<pair<int, int>> newQueue;
        while (!pageQueue.empty()) {
            auto p = pageQueue.front();
            pageQueue.pop();
            if (p.first != processId) {
                newQueue.push(p);
            }
        }
        pageQueue = move(newQueue);
    }

    processPageTables.erase(processId);
    ss << "Freed process " << processId << " and its pages.";
    log(NORMAL, ss.str());
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
void VirtualMemoryManager::runOptimalSimulation(const std::vector<std::pair<int, int>>& referenceString) {
    stringstream ss;
    if (policy != ReplacementPolicy::OPTIMAL) {
        ss << "Error: Optimal simulation can only be run with the OPTIMAL policy.";
        log(NORMAL, ss.str());
        return;
    }
    
    this->futureReferences = referenceString;
    this->currentReferenceIndex = -1;
    
    for (const auto& ref : referenceString) {
        this->currentReferenceIndex++;
        int processId = ref.first;
        int virtualPage = ref.second;
        stringstream ss_access;
        ss_access << "\n--- Accessing P" << processId << " VP" << virtualPage << " ---";
        log(VERBOSE, ss_access.str());
        accessPage(processId, virtualPage);
    }

    log(NORMAL, "\n=== Optimal Simulation Complete ===");
    printFrameTable();
    stringstream ss_faults;
    ss_faults << "Total Page Faults: " << getPageFaults();
    log(NORMAL, ss_faults.str());
}