#include "virtual_memory.hpp"
#include <iostream>
#include <queue>
#include <climits>
#include <sstream>
#include <iomanip>

using namespace std;



VirtualMemoryManager::VirtualMemoryManager(int memorySize, int pageSize, ReplacementPolicy policy)
    : pageSize(pageSize), pageFaults(0), accessCounter(0), policy(policy), clockHand(0)
{
    totalFrames = memorySize / pageSize;
    frameTable.resize(totalFrames, {NULL, -1});
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

void VirtualMemoryManager::allocateProcess(ProcessControlBlock& pcb)
{
    pcb.page_directory = PageDirectory();
    log(NORMAL, "Initialized page directory for process " + to_string(pcb.process_id) + ".");
}

// Set page permission
void VirtualMemoryManager::setPagePermissions(ProcessControlBlock& pcb, int virtualPageNumber, bool read, bool write, bool execute)
{
    
    int pdi = virtualPageNumber / PAGE_TABLE_SIZE;
    int pti = virtualPageNumber % PAGE_TABLE_SIZE;

    PageDirectory &pd = pcb.page_directory;

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

    log(VERBOSE, "Permissions for P" + to_string(pcb.process_id) + " VP " + to_string(virtualPageNumber) + " set to: R=" + (read ? "1" : "0") + " W=" + (write ? "1" : "0") + " X=" + (execute ? "1" : "0"));
}

// Access Page
void VirtualMemoryManager::accessPage(ProcessControlBlock& pcb, int virtualPageNumber, AccessType type)
{

    int pdi = virtualPageNumber / PAGE_TABLE_SIZE;
    int pti = virtualPageNumber % PAGE_TABLE_SIZE;

    log(DEBUG, "Translating VP " + to_string(virtualPageNumber) + " -> PDI: " + to_string(pdi) + ", PTI: " + to_string(pti));

    PageDirectory &pd = pcb.page_directory;

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
        log(VERBOSE, "Page fault at P" + to_string(pcb.process_id) + " VP " + to_string(virtualPageNumber));
        handlePageFault(pcb, virtualPageNumber, *pt, pti);
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
            log(NORMAL, "!!! PROTECTION FAULT: P" + to_string(pcb.process_id) + " attempted to " + (type == AccessType::WRITE ? "WRITE" : (type == AccessType::READ ? "READ" : "EXECUTE")) + " a page with no permission. Access denied.");
            return;
        }

        log(VERBOSE, "Page access successful for P" + to_string(pcb.process_id) + " VP " + to_string(virtualPageNumber) + ".");
        pte.lastAccessTime = accessCounter++;
        pte.referenced = true;

        int frame = pte.frameNumber;
        int physicalAddress = frame * pageSize;
        stringstream ss_pa;
        ss_pa << "-> Physical Address: " << physicalAddress << " (Frame " << frame << ")";
        log(DEBUG, ss_pa.str());
    }
}

void VirtualMemoryManager::handlePageFault(ProcessControlBlock& pcb, int virtualPageNumber, PageTable &pt, int pti) {
    pageFaults++;
    log(VERBOSE, "Handling page fault...");

    // Find a free frame first
    for (int i = 0; i < totalFrames; ++i) {
        if (frameTable[i].first == nullptr) {
            log(VERBOSE, "Found free frame " + to_string(i) + ".");
            frameTable[i] = {&pcb, virtualPageNumber};
            pt[pti].frameNumber = i;
            pt[pti].valid = true;
            pt[pti].lastAccessTime = accessCounter++;
            pt[pti].referenced = true;
            pt[pti].can_read = true;
            pt[pti].can_write = true;
            pt[pti].can_execute = false;
            if (policy == ReplacementPolicy::FIFO) {
                pageQueue.push({pcb.process_id, virtualPageNumber});
            }
            return;
        }
    }

    log(VERBOSE, "No free frames. Starting replacement...");
    int victimFrame = -1;

    // --- UPDATED REPLACEMENT LOGIC ---
    if (policy == ReplacementPolicy::FIFO) {
        if (!pageQueue.empty()) {
            auto toEvict = pageQueue.front();
            pageQueue.pop();
            // Find the frame containing the page to evict
            for(int i = 0; i < totalFrames; ++i) {
                if(frameTable[i].first->process_id == toEvict.first && frameTable[i].second == toEvict.second) {
                    victimFrame = i;
                    break;
                }
            }
        }
    }
    else if (policy == ReplacementPolicy::LRU) {
        unsigned long minAccessTime = ULONG_MAX;
        for (int i = 0; i < totalFrames; ++i) {
            ProcessControlBlock* victimPcb = frameTable[i].first;
            int vpn = frameTable[i].second;
            int victim_pdi = vpn / PAGE_TABLE_SIZE;
            int victim_pti = vpn % PAGE_TABLE_SIZE;
            if (victimPcb->page_directory.at(victim_pdi).pageTable->at(victim_pti).lastAccessTime < minAccessTime) {
                minAccessTime = victimPcb->page_directory.at(victim_pdi).pageTable->at(victim_pti).lastAccessTime;
                victimFrame = i;
            }
        }
    }
    // ... Clock policy would also need updating here ...

    // --- Common eviction logic ---
    if (victimFrame != -1) {
        ProcessControlBlock* victimPcb = frameTable[victimFrame].first;
        int victimVpn = frameTable[victimFrame].second;
        log(VERBOSE, "Evicting P" + to_string(victimPcb->process_id) + " VP" + to_string(victimVpn) + " from frame " + to_string(victimFrame) + ".");

        int victim_pdi = victimVpn / PAGE_TABLE_SIZE;
        int victim_pti = victimVpn % PAGE_TABLE_SIZE;
        PageTable* victimPT = victimPcb->page_directory.at(victim_pdi).pageTable;
        victimPT->at(victim_pti).valid = false;
        victimPT->at(victim_pti).frameNumber = -1;

        frameTable[victimFrame] = {&pcb, virtualPageNumber};
        pt[pti].frameNumber = victimFrame;
        pt[pti].valid = true;
        pt[pti].lastAccessTime = accessCounter++;
        pt[pti].referenced = true;
        pt[pti].can_read = true;
        pt[pti].can_write = true;
        pt[pti].can_execute = false;

        if (policy == ReplacementPolicy::FIFO) {
            pageQueue.push({pcb.process_id, virtualPageNumber});
        }
    } else {
        log(NORMAL, "CRITICAL ERROR: Could not determine a victim frame!");
    }
}

void VirtualMemoryManager::freeProcess(ProcessControlBlock& pcb)
{
    PageDirectory &pd = pcb.page_directory;

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
                    frameTable[pte.frameNumber] = {NULL, -1};
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
            if (p.first != pcb.process_id)
            {
                newQueue.push(p);
            }
            pageQueue.pop();
        }
        pageQueue = move(newQueue);
    }

    log(NORMAL, "Freed memory resources for process " + to_string(pcb.process_id) + ".");
}

void VirtualMemoryManager::printPageTable(const ProcessControlBlock& pcb) const
{
    cout << "\n=== Page Table for Process " << pcb.process_id << " ===\n";
    const PageDirectory &pd = pcb.page_directory;
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

void VirtualMemoryManager::printFrameTable() const
{
    cout << "\n=== Frame Table ===\n";
    cout << "Frame\tProcess\tPage\n";
    for (int i = 0; i < totalFrames; ++i)
    {
        if (frameTable[i].first == NULL)
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

void VirtualMemoryManager::displayMemoryLayout() const {
    const int frames_per_row = 8;
    std::cout << "\n--- Physical Memory Layout ---\n";

    for (int i = 0; i < totalFrames; ++i) {
        // Top border of the box
        std::cout << "+--------";
    }
    std::cout << "+\n";

    for (int i = 0; i < totalFrames; ++i) {
        // Content of the box
        if (frameTable[i].first != nullptr) {
            // Occupied frame (print in red)
            std::cout << "|\033[31m P" << std::setw(2) << frameTable[i].first->process_id
                      << ":V" << std::setw(2) << frameTable[i].second << "\033[0m ";
        } else {
            // Free frame (print in green)
            std::cout << "|\033[32m  Free  \033[0m ";
        }
    }
    std::cout << "|\n";

    for (int i = 0; i < totalFrames; ++i) {
        // Frame number
        std::cout << "| Frame " << std::setw(2) << i << " ";
    }
    std::cout << "|\n";

    for (int i = 0; i < totalFrames; ++i) {
        // Bottom border of the box
        std::cout << "+--------";
    }
    std::cout << "+\n";
}

