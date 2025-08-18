#include "virtual_memory.hpp"
#include <iostream>
#include <queue>
#include <climits>
#include <sstream>

using namespace std;

const int PAGE_TABLE_SIZE = 1024;

VirtualMemoryManager::VirtualMemoryManager(int memorySize, int pageSize, ReplacementPolicy policy)
    : pageSize(pageSize), pageFaults(0), accessCounter(0), policy(policy), clockHand(0), currentReferenceIndex(0)
{
    totalFrames = memorySize / pageSize;
    frameTable.resize(totalFrames, {-1, -1});
}

void VirtualMemoryManager::log(LogLevel level, const string &message) const
{
    if (current_log_level >= level)
    {
        cout << message << endl;
    }
}

void VirtualMemoryManager::setLogLevel(LogLevel level)
{
    current_log_level = level;
}

void VirtualMemoryManager::allocateProcess(int processId)
{
    stringstream ss;
    if (processPageDirectories.find(processId) != processPageDirectories.end())
    {
        ss << "Error: Process " << processId << " already exists.";
        log(NORMAL, ss.str());
        return;
    }

    processPageDirectories[processId] = PageDirectory();

    ss << "Allocated process " << processId << " with a new page directory.";
    log(NORMAL, ss.str());
}

// Set page permission
void VirtualMemoryManager::setPagePermissions(int processId, int virtualPageNumber, bool read, bool write, bool execute)
{
    if (processPageDirectories.find(processId) == processPageDirectories.end())
    {
        log(NORMAL, "Error: Cannot set permissions for non-existent process " + to_string(processId));
        return;
    }

    int pdi = virtualPageNumber / PAGE_TABLE_SIZE;
    int pti = virtualPageNumber % PAGE_TABLE_SIZE;

    PageDirectory &pd = processPageDirectories.at(processId);

    if (pd.find(pdi) == pd.end() || !pd.at(pdi).valid)
    {
        log(DEBUG, "Creating page table for PDI " + to_string(pdi) + " to set permissions.");
        PageTable *newPageTable = new PageTable();
        pd[pdi].pageTable = newPageTable;
        pd[pdi].valid = true;
    }

    PageTable *pt = pd.at(pdi).pageTable;
    PageTableEntry &pte = (*pt)[pti];

    pte.can_read = read;
    pte.can_write = write;
    pte.can_execute = execute;

    log(VERBOSE, "Permissions for P" + to_string(processId) + " VP " + to_string(virtualPageNumber) + " set to: R=" + (read ? "1" : "0") + " W=" + (write ? "1" : "0") + " X=" + (execute ? "1" : "0"));
}

// Access Page
void VirtualMemoryManager::accessPage(int processId, int virtualPageNumber, AccessType type)
{
    stringstream ss;
    if (processPageDirectories.find(processId) == processPageDirectories.end())
    {
        ss << "Error: Process " << processId << " not found.";
        log(NORMAL, ss.str());
        return;
    }

    int pdi = virtualPageNumber / PAGE_TABLE_SIZE;
    int pti = virtualPageNumber % PAGE_TABLE_SIZE;

    log(DEBUG, "Translating VP " + to_string(virtualPageNumber) + " -> PDI: " + to_string(pdi) + ", PTI: " + to_string(pti));

    PageDirectory &pd = processPageDirectories.at(processId);

    if (pd.find(pdi) == pd.end() || pd.at(pdi).valid == false)
    {
        log(VERBOSE, "Directory Miss for PDI " + to_string(pdi) + ". Allocating new page table.");
        PageTable *newPageTable = new PageTable();
        pd[pdi].pageTable = newPageTable;
        pd[pdi].valid = true;
    }

    PageTable *pt = pd.at(pdi).pageTable;

    if (pt->find(pti) == pt->end() || pt->at(pti).valid == false)
    {
        ss << "Page fault at P" << processId << " VP " << virtualPageNumber;
        log(VERBOSE, ss.str());
        handlePageFault(processId, virtualPageNumber, *pt, pti);
    }
    else
    {
        PageTableEntry &pte = pt->at(pti);

        bool permission_granted = false;
        switch (type)
        {
        case AccessType::READ:
            if (pte.can_read)
                permission_granted = true;
            break;
        case AccessType::WRITE:
            if (pte.can_write)
                permission_granted = true;
            break;
        case AccessType::EXECUTE:
            if (pte.can_execute)
                permission_granted = true;
            break;
        }

        if (!permission_granted)
        {
            log(NORMAL, "!!! PROTECTION FAULT: P" + to_string(processId) + " attempted to " + (type == AccessType::WRITE ? "WRITE" : (type == AccessType::READ ? "READ" : "EXECUTE")) + " a page with no permission. Access denied.");
            return;
        }

        ss << "Page access successful for P" << processId << " VP " << virtualPageNumber << ".";
        log(VERBOSE, ss.str());

        pt->at(pti).lastAccessTime = accessCounter++;
        pt->at(pti).referenced = true;

        int frame = pt->at(pti).frameNumber;
        int physicalAddress = frame * pageSize;
        stringstream ss_pa;
        ss_pa << "-> Physical Address: " << physicalAddress << " (Frame " << frame << ")";
        log(DEBUG, ss_pa.str());
    }
}

void VirtualMemoryManager::handlePageFault(int processId, int virtualPageNumber, PageTable &pt, int pti)
{
    pageFaults++;
    log(VERBOSE, "Handling page fault...");

    // Try to find a free frame first
    for (int i = 0; i < totalFrames; ++i)
    {
        if (frameTable[i].first == -1)
        {
            log(VERBOSE, "Found free frame " + to_string(i) + ".");
            frameTable[i] = {processId, virtualPageNumber};
            pt[pti].frameNumber = i;
            pt[pti].valid = true;
            pt[pti].lastAccessTime = accessCounter++;
            pt[pti].referenced = true;
            if (policy == ReplacementPolicy::FIFO)
            {
                pageQueue.push({processId, virtualPageNumber});
            }
            return;
        }
    }

    // If no free frame, begin replacement logic
    log(VERBOSE, "No free frames. Starting replacement...");
    int victimFrame = -1;

    if (policy == ReplacementPolicy::LRU)
    {
        unsigned long minAccessTime = ULONG_MAX;
        for (int i = 0; i < totalFrames; ++i)
        {
            int pid = frameTable[i].first;
            int vpn = frameTable[i].second;
            int victim_pdi = vpn / PAGE_TABLE_SIZE;
            int victim_pti = vpn % PAGE_TABLE_SIZE;
            if (processPageDirectories.at(pid).at(victim_pdi).pageTable->at(victim_pti).lastAccessTime < minAccessTime)
            {
                minAccessTime = processPageDirectories.at(pid).at(victim_pdi).pageTable->at(victim_pti).lastAccessTime;
                victimFrame = i;
            }
        }
    }
    // ... other policies like FIFO, CLOCK, OPTIMAL would need similar logic adjustments ...

    // --- Common eviction logic for ALL replacement policies ---
    if (victimFrame != -1)
    {
        int victimPid = frameTable[victimFrame].first;
        int victimVpn = frameTable[victimFrame].second;
        log(VERBOSE, "Evicting P" + to_string(victimPid) + " VP" + to_string(victimVpn) + " from frame " + to_string(victimFrame) + ".");

        int victim_pdi = victimVpn / PAGE_TABLE_SIZE;
        int victim_pti = victimVpn % PAGE_TABLE_SIZE;
        PageTable *victimPT = processPageDirectories.at(victimPid).at(victim_pdi).pageTable;
        victimPT->at(victim_pti).valid = false;
        victimPT->at(victim_pti).frameNumber = -1;

        frameTable[victimFrame] = {processId, virtualPageNumber};
        pt[pti].frameNumber = victimFrame;
        pt[pti].valid = true;
        pt[pti].lastAccessTime = accessCounter++;
        pt[pti].referenced = true;

        if (policy == ReplacementPolicy::FIFO)
        {
            pageQueue.push({processId, virtualPageNumber});
        }
    }
    else
    {
        log(NORMAL, "CRITICAL ERROR: Could not determine a victim frame!");
    }
}

void VirtualMemoryManager::freeProcess(int processId)
{
    stringstream ss;
    if (processPageDirectories.find(processId) == processPageDirectories.end())
    {
        ss << "Error: Process " << processId << " not found.";
        log(NORMAL, ss.str());
        return;
    }

    PageDirectory &pd = processPageDirectories.at(processId);

    for (auto const &pde_pair : pd)
    {
        auto const &pde = pde_pair.second;
        if (pde.valid && pde.pageTable != nullptr)
        {
            PageTable *pt = pde.pageTable;
            for (auto const &pte_pair : *pt)
            {
                auto const &pte = pte_pair.second;
                if (pte.valid)
                {
                    frameTable[pte.frameNumber] = {-1, -1};
                }
            }
            delete pt;
        }
    }

    if (policy == ReplacementPolicy::FIFO)
    {
        queue<pair<int, int>> newQueue;
        while (!pageQueue.empty())
        {
            auto p = pageQueue.front();
            if (p.first != processId)
            {
                newQueue.push(p);
            }
            pageQueue.pop();
        }
        pageQueue = move(newQueue);
    }

    processPageDirectories.erase(processId);
    ss << "Freed process " << processId << " and its resources.";
    log(NORMAL, ss.str());
}

void VirtualMemoryManager::printPageTable() const
{
    cout << "\n=== Page Tables (Multi-Level) ===\n";
    for (const auto &proc_pair : processPageDirectories)
    {
        int pid = proc_pair.first;
        const PageDirectory &pd = proc_pair.second;
        cout << "Process " << pid << ":\n";

        for (const auto &pde_pair : pd)
        {
            if (pde_pair.second.valid)
            {
                int pdi = pde_pair.first;
                const PageTable *pt = pde_pair.second.pageTable;
                cout << "  PDI [" << pdi << "] -> Page Table:\n";
                cout << "    PTI\tFrame\tValid\n";

                for (const auto &pte_pair : *pt)
                {
                    int pti = pte_pair.first;
                    const PageTableEntry &pte = pte_pair.second;
                    cout << "    " << pti << "\t" << pte.frameNumber << "\t" << (pte.valid ? "Yes" : "No") << "\n";
                }
            }
        }
        cout << "\n";
    }
}

void VirtualMemoryManager::printFrameTable() const
{
    cout << "\n=== Frame Table ===\n";
    cout << "Frame\tProcess\tPage\n";
    for (int i = 0; i < totalFrames; ++i)
    {
        if (frameTable[i].first == -1)
        {
            cout << i << "\tFree\t-\n";
        }
        else
        {
            cout << i << "\t" << frameTable[i].first
                 << "\t" << frameTable[i].second << "\n";
        }
    }
}

void VirtualMemoryManager::runOptimalSimulation(const std::vector<std::pair<int, int>> &referenceString)
{
    log(NORMAL, "Optimal simulation needs to be updated for multi-level paging.");
}